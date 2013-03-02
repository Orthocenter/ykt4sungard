/* --------------------------------------------
 * ��������: F930067.sqc
 * ��������: 2007-07-24
 * ��������: ����
 * �汾��Ϣ: 1.0.0.0
 * ������: ָ����Ƴ�������
 * --------------------------------------------*/
#define _IN_SQC_                    
#include <string.h>
#include <stdio.h>
#include "pubfunc.h"
#include "pubdb.h"
#include "pubdef.h"
#include "errdef.h"
#include "dbfunc.h"
#include "dbfunc_foo.h"
#include "busqc.h"

int F930067(TRUSERID *handle,int iRequest,ST_PACK *in_pack,int *pRetCode,char *szMsg)
{
	int ret = 0;
	int cnt=0;
	int mesid=0;
	char sysdatetime[21]="";
       T_t_tif_meslist tMesList;
	T_t_device tDevice;

	ST_CPACK aPack;
	ST_PACK *out_pack = &(aPack.pack);
	ResetNormalCPack(&aPack,0,1);

	memset(&tMesList,0,sizeof(tMesList));
	memset(&tDevice,0,sizeof tDevice);
	
	if(strlen(in_pack->sdate0)==0)
	{
		if(in_pack->lcert_code <= 0)
		{
			*pRetCode=E_DEVPHYID_NULL;
			goto L_RETU;
		}
		tMesList.devid = in_pack->lcert_code;
	}
	else
	{
		if((ret = get_devinfo_by_phyid(in_pack->sdate0,&tDevice)))
		{
			*pRetCode = ret;
			goto L_RETU;
		}
		tMesList.devid = tDevice.deviceid;
	}
	
	getsysdatetime(sysdatetime);
	AddXmlItemInt(tMesList.incontent,XML_KEY_FTFLAG,in_pack->lvol3);	

	tMesList.funid = 930067;
	tMesList.level = 2;
	ret=AddMsgLst(&tMesList);
	if(ret)
	{
		*pRetCode=ret;
		writelog(LOG_ERR,"AddMsgLst err[%d]",ret);
		goto L_RETU;
	}	
	if(iRequest!=tMesList.funid)
		return 0;	
	ret=db_commit();
	if(ret)
	{
		writelog(LOG_ERR,"db_commit ret[%d]",ret);
		*pRetCode=E_DB_COMMIT;
		goto  L_RETU;
	}
	mesid=tMesList.mesid;
	for(cnt=0;(cnt<10)&&(tMesList.ecode!=0);cnt++)
	{
		sleep(1);
		memset(&tMesList,0,sizeof(tMesList));
		ret=DB_t_tif_meslist_read_by_mesid(mesid, &tMesList);
		if(ret)
		{
			writelog(LOG_ERR,"DB_t_tif_meslist_read_by_mesid err[%d]",ret);
			*pRetCode=E_DB_MESLIST_R;
			goto L_RETU;
		}
	}
	switch(tMesList.ecode)
	{
		case 0:		//�ɹ�
			des2src(out_pack->vsvarstr0,"�ɹ�");
			break;
		case 9999:		//����δ����
			des2src(out_pack->vsvarstr0,"ǰ�û���Ӧ��");
			break;
		default:			//�����Ѿ�����,�������ʧ��
			des2src(out_pack->vsvarstr0,tMesList.emsg);				
			break;
	}	
	SetCol(handle,0);
	SetCol(handle,F_VSVARSTR0,0);
	PutRow(handle,out_pack,pRetCode,szMsg);
	return 0;
L_RETU:
	return  -1;	
}


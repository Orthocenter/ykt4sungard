#ifndef BDFUNC_INC
#define BDFUNC_INC
/*  ��ÿ��ҵ���������б�������������һ�У���ϸ�����ʾ�ĺ��壬��BDefine.h�е�������
   {1000     ,FTestProc   ,"Demo for SQC ҵ����������д����"             ,"CYH Demo"          ,1     ,false,0,0,0,0,0},
   {���ܺ�   ,���̺���>    ,"��������"                                     ,"�����Ա����"       ,���ȼ�,false,0,0,0,0,0},
*/
// �����г����кϷ���ҵ�������̺����б���
#ifdef __cplusplus
extern "C" {
#endif


// ���ܺź���ԭ������, ����BU�ص�
///////////////////////////////////////////////////////////////////////////////////
int F851001(TRUSERID *handle, int iRequest, ST_PACK *rPack, int *pRetCode, char *szMsg);
int F851002(TRUSERID *handle, int iRequest, ST_PACK *rPack, int *pRetCode, char *szMsg);
int F851003(TRUSERID *handle, int iRequest, ST_PACK *rPack, int *pRetCode, char *szMsg);
///////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

// �������ݱ�Ϊ��ҵ������Ԫ�ܴ��������кϷ�ҵ�������̼�����ز���
TBDefine g_XBDefines[]=
{
	INSERT_FUNCTION(851001 ,F851001, "������֧��", "tc", 1)
	INSERT_FUNCTION(851002 ,F851002, "������֧������", "tc", 1)
	INSERT_FUNCTION(851003 ,F851003, "������֧����ѯ", "tc", 1)
	INSERT_FUNCTION(0,NULL,"END BPFunctions List","CYH Marked",0)  // the last mark line��Don't remove this line
};
#endif
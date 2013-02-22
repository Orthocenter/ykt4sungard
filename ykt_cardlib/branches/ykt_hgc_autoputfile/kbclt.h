/********************************************************************
	created:	2006/12/29
	created:	29:12:2006   11:17
	filename: 	D:\proj\ksbass\bcclink\KBDClt\KBDClt.h
	file path:	D:\proj\ksbass\bcclink\KBDClt
	file base:	KBDClt
	file ext:	h
	author:		CHENYH
	
	purpose:	����֤ȯ�ӿڲ�Ҫ��Ϊ���ܹ����������ṩ���ں�KSMBASS�ܹ��Կͻ���ʽ����ͨѶ���������Ľӿڿ⡣
         ���ͻ��˽ӿ�ֻ�ܴ��������Ӧ�÷���������һ���������ܺţ�
         ���ͻ��˾��KBDCHandleΪ���̰߳�ȫ�����󲿷ֲ���������ֻ��һ���߳���ʹ��
*********************************************************************/
#ifndef KBDCLT_H 
#define KBDCLT_H

#include "LogFile.h"
#define MAXFVLENGTH  1024   // ��KBDCGetFieldValue��fieldvalueԤ������ֽ���

#ifdef __cplusplus
extern "C" {
#endif
typedef void * KBDCHandle;

CLogFile g_log_file("file_log");
#define DEBUG_CARD_MSGOUT(ret, msg) { g_log_file.WriteLogEx(1000, "FILE:%s---LINE:%d---FUNCNAME:%s---RET:%d---MESSAGE:%s", __FILE__, __LINE__, __FUNCTION__, ret, msg); }

// ������: KBDCGetLocalLicensePair
// ���  : ������ 2006-12-29 17:54:54
// ����  : Ϊ�˸��ͻ��˳���İ���ҳ����ʾ���ذ�װ����Ȩ�ŵ�������
// ����  : unsigned int *p1 [OUT]: ���ر��ذ�װ��Ȩ��������1
// ����  : unsigned int *p2 [OUT]: ���ر��ذ�װ��Ȩ��������2
void KBDCGetLocalLicensePair(unsigned int *p1, unsigned int *p2);


// ������: KBDCInit
// ���  : ������ 2006-12-29 11:37:21
// ����  : �ͻ��ӿڿ���Ի�
// ����  : int 0 �� �ɹ���<0: ����ʧ������
// ����  : int debug [IN]: 0 �� ��Ҫ������Ϣ�� !=0 - �򿪵��Ը����ļ�
// ����  : const char *license [IN]: ��Ȩ�ţ���Ҫ��Kingstar������Ȩ��
// ����  : char *eMsg [OUT]�������ز�Ϊ 0 ��ʱ�򣬷��صĴ�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
int KBDCInit(int debug, const char *license, char *eMsg);


// ������: KBDCExit
// ���  : ������ 2006-12-29 13:16:48
// ����  : �˳��ӿڿ�
// ����  : void 
void KBDCExit();


// ������: NewKBDCHandle
// ���  : ������ 2006-12-29 13:19:29
// ����  : ��ȡ�ͻ��˽ӿھ��
// ����  : KBDCHandle : NULL - ʧ�ܣ����� - �ɹ�
// ����  : char *eMsg ����ʧ�ܷ��ص�ʱ�򣬷��ش�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
KBDCHandle KBDCNewHandle(char *eMsg);


// ������: DeleteKBDCHandle
// ���  : ������ 2006-12-29 13:22:43
// ����  : �ͷſͻ��˽ӿھ��
// ����  : void 
// ����  : KBDCHandle *chandle : ͨ��NewKBDCHandle��õĿͻ��˽ӿھ��
void KBDCDeleteHandle(KBDCHandle chandle);


// ������: KBDCBeginRequest
// ���  : ������ 2006-12-29 13:28:14
// ����  : ׼��һ������
// ����  : int : 0 �ɹ���<0 ʧ��
// ����  : KBDCHandle chandle [IN]: ͨ��NewKBDCHandle��õĿͻ��˽ӿھ��
// ����  : int reqno [IN]: ����Ĺ��ܺ�
// ����  : char *eMsg [OUT]: ��ʧ�ܷ��ص�ʱ�򣬷��ش�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
int KBDCBeginRequest(KBDCHandle chandle, int reqno, char *eMsg);


// ������: KBDCSetFieldValue
// ���  : ������ 2006-12-29 15:18:38
// ����  : �������������е�ָ���ֶ�ֵ
// ����  : int : 0 �ɹ���<0 ʧ��
// ����  : KBDCHandle chandle [IN]: �Ѿ�������KBDCBeginRequest�Ŀͻ��˽ӿھ��
// ����  : const char *fieldname [IN]: ָ�����ֶ�������vsmess
// ����  : const char *fieldvalue [IN]: ָ�����ֶε�ֵ�����ַ�����ʾ
int KBDCSetFieldValue(KBDCHandle chandle, const char *fieldname,const char *fieldvalue);


// ������: KBDCCallSvr
// ���  : ������ 2006-12-29 15:54:57
// ����  : ����������׼���õ������͸��������ˣ��ȴ�����˵�Ӧ�𷵻�
// ����  : int : >=0 �ɹ��� <0 ʧ�ܣ����ش�����
// ����  : KBDCHandle chandle [IN]: ͨ��NewKBDCHandle��õĿͻ��˽ӿھ���������Ѿ�ͨ��KBDCBeginRequest��KBDCSetFieldValue
//         ׼��������������
// ����  : int waittime [IN]: �ȴ�Ӧ���ʱ�䣬�����
// ����  : char *eMsg [OUT]:  ��ʧ�ܷ��ص�ʱ�򣬷��ش�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
int KBDCCallSvr(KBDCHandle chandle, int waittime, char *eMsg);


// ������: KBDCGetRetCode
// ���  : ������ 2006-12-29 16:00:54
// ����  : ��ȡKBDCCallSvr��ķ���˷��صķ�����
// ����  : int : >=0 ���صķ����룬�ڵ���KBDCCallSvr�ɹ����ȡ��<0: ���� 
// ����  : KBDCHandle chandle [IN]: �����Ŀͻ��˽ӿھ��
int KBDCGetRetCode(KBDCHandle chandle);


// ������: KBDCNextRetRecord
// ���  : ������ 2006-12-29 16:04:49
// ����  : �ڵ���KBDCCallSvr��ȱʡ�ķ��ؼ�¼���Ϊ0�������KBDCGetFieldValue����һ����¼�е����ݣ�����Ҫ���ñ�����
// ����  : int : >=0 �ɹ������ڲ�һ��ֵ��<0 ʧ�ܣ�(-1)��ʾû����һ����¼�ˣ�������ԭ��
// ����  : KBDCHandle chandle [IN]: �����Ŀͻ��˽ӿھ��
// ����  : char *eMsg [OUT]:  ��ʧ�ܷ��ص�ʱ�򣬷��ش�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
int KBDCNextRetRecord(KBDCHandle chandle, char *eMsg);


// ������: KBDCGetFieldValue
// ���  : ������ 2006-12-29 16:09:29
// ����  : ȡ��ǰ��¼�еĸ����ֶε�ֵ
// ����  : int : >=0 �ɹ���<0 ʧ�ܣ���ʾ������
// ����  : KBDCHandle chandle [IN]: �����Ŀͻ��˽ӿھ��
// ����  : const char *fieldname [IN]: ָ��ȡֵ���ֶ�������vsmess
// ����  : char *fieldvalue [OUT]: ���ص��ֶ�ֵ�� ���÷�Ԥѡ����1024�ֽڿռ�
// ����  : char *eMsg [OUT]:  ��ʧ�ܷ��ص�ʱ�򣬷��ش�����Ϣ�� ���÷�Ԥѡ����1024�ֽڿռ�
int KBDCGetFieldValue(KBDCHandle chandle, const char *fieldname, char *fieldvalue, char *eMsg);


#ifdef __cplusplus
}
#endif
#endif
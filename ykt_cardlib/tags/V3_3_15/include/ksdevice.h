#ifndef _KSDEVICE_H_
#define _KSDEVICE_H_

#include <strstream>
#include <sstream>
#include "logfile.h"

#ifndef uint8
#define uint8  unsigned char
#endif

#ifdef WIN32
#define KSAPI __stdcall
#else
#ifdef KSAPI
#undef KSAPI
#define KSAPI
#endif 
#endif

//////////////////////////////////////////////////////////////////////////
// 错误码定义
#define KS_SUCCESS 0
#define KS_PORT_NOTOPEN 1000	//端口未打开
#define KS_REQUESTCARD 1001		// 寻卡失败
#define KS_LOGINCARD 1002		// 登陆卡片失败
#define KS_WRITECARD 1003		// 写卡失败
#define KS_READCARD 1004		// 读卡失败
#define KS_UNCONFIRM 1005		// 不能确认的写卡操作
#define KS_WRITEPURSE 1006		// 写卡钱包失败
#define KS_OPENDEVICE 1007		// 打卡设备失败
#define KS_BLOCK_OVERFLOW 1008	// 读写块超过范围
#define KS_PARAMERR 1009		// 读卡器参数错误
#define KS_CMDERROR 1010		// CPU指令失败
#define KS_CMDFAILD 1011		// CPU指令返回错误
#define KS_NOTSUPPORT 1012		// 不支持的操作
#define KS_NOPARAMFOUND 1013	// 未找到指定参数
#define KS_PURSEERROR 1014		// 卡钱包格式错误
#define KS_EXCEEDMONEY	1015	// 充值金额超过限制
#define KS_UNAUTH_TERM 1016		// 终端未授权
#define KS_PURSECRCERR	1017	// 卡钱包数据块校验错
#define KS_COMM_ERR 1099		// 未知错误

#pragma pack(1)
// 读卡器接口类型,0 串口, 1 USB接口
enum {KS_READER_COM=0,KS_READER_USB=1};

enum {CARDSLOT_1=1,CARDSLOT_2=2,CARDSLOT_3=3,CARDSLOT_4=4,CARDSLOT_5=5,CARDSLOT_RF=99};

typedef void * DEV_HANDLE;
typedef struct _tagKsReaderDev
{
	int port;
	int baud;
	int cpuport; // cpu卡座号
	int cputype; // 卡协议类型，0表示T=0 , 1表示T=1
	int cpubaud; // cpu卡波特率 9600, 38400
	int samport; // sam卡座号
	int devtype; // 读卡器类型
	int no;		// 读卡器编号
	int cardtype; // 卡类型, 输出参数，由寻卡时获取
}ks_reader_dev_t;

typedef enum {KS_MFCARD = 1,KS_CPUCARD,KS_FIXCARD,KS_PSAM,KS_KEYCARD,KS_SIMCARD} KS_CARD_TYPE;
typedef enum {CPUCARD_A  = 0,CPUCARD_B = 1} KS_CPUCARD_TYPE;

typedef enum {MF_KEYA = 1, MF_KEYB = 2,MF_ORGINKEY=3,MF_INITKEY=4} KS_MF_KEYTYPE;

#define MF_MAX_SECTNO 48
#define MF_MAX_BLOCK 4
#define MF_MAX_BLOCK_SIZE 16*MF_MAX_BLOCK
typedef enum {MF_BLK_NO_CRC = 0, MF_BLK_CRC_SUM = 1,MF_BLK_CRC_SUM_ALLOW_ZERO = 2, MF_BLK_CRC_OR = 3, MF_CRC8 } KS_MF_BLOCK_CRC;
typedef struct 
{
	unsigned char block_used[MF_MAX_BLOCK];
	unsigned char block_crc[MF_MAX_BLOCK];	
}ks_mf_sect_block_t;
typedef struct _tagKsCardInfo
{
	KS_CARD_TYPE card_type;
	char phyid[17];
	int card_size;
	int mf_min_sect;
	int mf_max_sect;
	uint8 sect_login[MF_MAX_SECTNO];
	ks_mf_sect_block_t block_info[MF_MAX_SECTNO];
}ks_card_info_t;

typedef struct _tagCpuCmd
{
	uint8 cmd_type; // 0 表示 原始字符格式 , 1 表示 hex 格式
	uint8 send_len;
	uint8 *send_buf;
	uint8 recv_len;
	uint8 *recv_buf;
	uint8 timeout; //  超时时间,单位为 秒
	int	 cmd_retcode; // 执行指令返回值
	char cmd_retmsg[256];//错误信息
}ks_cpu_cmd_t;

typedef struct _tagDeviceOp {
	const char *version;
	const char *devname;
	/* common */
	int (KSAPI *open_device)(ks_reader_dev_t *dev);
	int (KSAPI *close_device)(ks_reader_dev_t *dev);
	/* 只负责寻卡 不复位 */
	int (KSAPI *request_card)(ks_reader_dev_t *dev,char *phyid,KS_CARD_TYPE t);
	int (KSAPI *halt)(ks_reader_dev_t *dev);
	int (KSAPI *beep)(ks_reader_dev_t *dev);
	int (KSAPI *beeperr)(ks_reader_dev_t *dev);
	int (KSAPI *reset)(ks_reader_dev_t *dev,size_t msec);
	/* mifare one card */
	int (KSAPI *login_card)(ks_reader_dev_t *dev,ks_card_info_t *card,int sect_no,KS_MF_KEYTYPE keytype,const uint8 *key);
	int (KSAPI *read_block)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no,uint8 *block);
	int (KSAPI *write_block)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no,const uint8 *block);
	int (KSAPI *decrement_card)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no,int amount);
	int (KSAPI *increment_card)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no,int amount);
	int (KSAPI *transform_card)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no);
	int (KSAPI *restore_card)(ks_reader_dev_t *dev,ks_card_info_t *card,int block_no);
	/* cpucard */
	int (KSAPI *config_card)(ks_reader_dev_t *dev,KS_CPUCARD_TYPE t);
	int (KSAPI *procard_cmd)(ks_reader_dev_t *dev,ks_cpu_cmd_t *cmd);
	/* psam card */
	/* 对CPU卡上电复位 */
	int (KSAPI *cpucard_poweron)(ks_reader_dev_t *dev,KS_CARD_TYPE t,uint8 *len,uint8 *buf);
	int (KSAPI *cpucard_setpara)(ks_reader_dev_t *dev);
	/* cpu and psam */
	int (KSAPI *cpucard_cmd)(ks_reader_dev_t *dev,ks_cpu_cmd_t *cmd,KS_CARD_TYPE t);
	int (KSAPI *get)(ks_reader_dev_t *dev,ks_cpu_cmd_t *cmd,KS_CARD_TYPE t);
	char* (KSAPI *GetErrMsg)(char *errmsg);
	int (KSAPI *card_type)();
	void (KSAPI *set_logger)(void *para);
}ks_device_op_t;

#pragma pack()

#define DECL_DLLFUNC(r,n,p) \
	typedef r (KSAPI *LP##n) p

#define DEF_DLLFUNC(n) \
	LP##n n = NULL

#define LOAD_DLLFUNC(m,n) do \
{n = (LP##n)GetProcAddress(m,#n); if(n==NULL) return -1;} while (0);


#define TRACE_ERR(message) do \
{ std::ostringstream oss;oss<<message; SetErrMsg(oss.str().c_str(),__FILE__,__LINE__);}while(0)

//////////////////////////////////////////////////////////////////////////
// 硬件层接口
DECL_DLLFUNC(int,register_device_op,(ks_device_op_t *op));


#endif // _KSDEVICE_H_

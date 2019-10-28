#pragma once

#ifdef __GGMSG_LIB__
#define __ggmsg_interface__ __declspec(dllexport)
#else
#define __ggmsg_interface__ __declspec(dllimport)

#endif // __NET_MSG_LIB__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// �������ӵĽ��֪ͨ
typedef int(*FnOnPositiveConnect)(int nServiceID, int nConnectID);

// ��������֪ͨ
// nSeviceID �������ӵķ���ID
typedef int(*FnOnPassiveConnect)(int nSeviceID, int nConnectID);

typedef int(*FnOnReceiveMsg)(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen);

__int64 __ggmsg_interface__ ggmsg_Create();

void __ggmsg_interface__ ggmsg_Destory(__int64 c);

void __ggmsg_interface__ ggmsg_Start(__int64 c, int nServiceID, short port,
	FnOnPassiveConnect fnOnPassiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg);

// ������������
int __ggmsg_interface__ ggmsg_Connect(__int64 c, const char *szHost, short sPort,
	FnOnPositiveConnect fnOnPositiveConnect,
	FnOnReceiveMsg fnOnReceiveMsg);

void __ggmsg_interface__ ggmsg_Stop(__int64 c);


// ��ָ������������
int __ggmsg_interface__ ggmsg_SendToService(__int64 c, int nServiceID, const void *pData, int nDataLen);

// ��ָ�����ӷ�������
int __ggmsg_interface__ ggmsg_SendToConnect(__int64 c, int nConnectID, const void *pData, int nDataLen);

#ifdef __cplusplus
}
#endif // __cplusplus

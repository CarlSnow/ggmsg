//3DES.h : ͷ�ļ�
//

#define ENCRYPT 0
#define DECRYPT 1

#ifndef _WINDOWS_
#include "windows.h"
#endif

typedef bool (*PSubKey)[16][48];

//3DES Class
class C3DES
{
public:
	bool DoDES(char *Out, char *In, long datalen, const char *Key, int keylen, bool Type);

private:
	const static char Table_IP[64];			// ��ʼ�û�
	const static char Table_InverseIP[64];	// δ�û�
	const static char Table_E[48];			// ��չ�û�
	const static char Table_PC1[56];		// ��Կ��ʼ�û� 
	const static char Table_Moveleft[16];	// ��������
	const static char Table_PC2[48];		// ��Կѹ���û� 
	const static char Box_S[8][4][16];		// S���û�
	const static char Table_P[32];			// P���û�

	bool SubKey[2][16][48];		//������Կ��16������Կ 
	char Tmp[256];
	char deskey[16];

	// ͨ���û�����
	void Transform(bool *Out, bool *In, const char *Table, int len);
	// ������� 
	void Xor(bool *InA, const bool *InB, int len);
	// ѭ������ 
	void MoveLeft(bool *In, int len, int loop);
	// �ֽ�ת����λ 
	void Byte2Bit(bool *Out, const char *In, int bits);
	// λת���ֽ� 
	void Bit2Byte(char *Out, const bool *In, int bits);
	// S���û�
	void funS(bool Out[32], const bool In[48]);
	// F���� 
	void funF(bool In[32], const bool Ki[48]);
	// ��������Կ 
	void MakeSubKey(PSubKey pSubKey, const char Key[8]);
	// ������Կ 
	void MakeKey(const char* Key, int len);
	// һ��DES��/���� 
	void DES(char Out[8], char In[8], const PSubKey pSubKey, bool Type);
};
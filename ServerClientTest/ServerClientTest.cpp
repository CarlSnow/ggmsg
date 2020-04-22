﻿// ServerClientTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "../ggmsg/ggmsg.h"

std::mutex connectIDLock;
std::vector<int> clientConnectIDList;

int OnPositiveConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveConnect " << nServiceID << " " << nConnectID << "\n";
	return 0;
}

int OnPositiveDisConnect(int nServiceID, int nConnectID)
{
	std::cout << "OnPositiveDisConnect " << nServiceID << " " << nConnectID << "\n";
	return 0;
}

// 被动连接通知
// nSeviceID 发起连接的服务ID
int OnPassiveConnect(int nSeviceID, int nConnectID)
{
	std::lock_guard<std::mutex> lk(connectIDLock);
	clientConnectIDList.push_back(nConnectID);

	std::cout << "OnPassiveConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnPassiveDisConnect(int nSeviceID, int nConnectID)
{
	std::lock_guard<std::mutex> lk(connectIDLock);
	auto it = clientConnectIDList.begin();
	for (; it != clientConnectIDList.end(); ++it) {
		clientConnectIDList.erase(it);
		break;
	}

	std::cout << "OnPassiveDisConnect " << nSeviceID << " " << nConnectID << "\n";
	return 0;
}

int OnServerReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

int OnReceiveMsg(int nServiceID, int nConnectID, const void *pMsg, int nMsgLen)
{
	std::cout << "from Service:" << nServiceID << " ConnectID:" << nConnectID << ", msg content:" << (char*)pMsg << "\n";
	return 0;
}

void TestClientServer() {
	auto server = ggmsg_Create();
	auto client = ggmsg_Create();

	{
		ggmsg_Start(server, 9, 9009, OnPassiveConnect, OnPassiveDisConnect, OnServerReceiveMsg);
	}

	{
		std::cout << "Client Start!\n";

		ggmsg_Connect(client, "127.0.0.1", 9009, OnPositiveConnect, OnPositiveDisConnect, OnReceiveMsg);
		char msg[] = "hello server";
		int nMsgLen = strlen(msg) + 1;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		for (int i = 0; i < 100; ++i) {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			ggmsg_SendToService(client, 9, msg, nMsgLen);
		}
	}
	
	{
		char msg[] = "hello client";
		int nMsgLen = strlen(msg) + 1;
		for (int i = 0; i < 100; ++i) {
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::lock_guard<std::mutex> lk(connectIDLock);
			for (int cid : clientConnectIDList) {
				ggmsg_SendToConnect(server, cid, msg, nMsgLen);
			}
		}
	}

	ggmsg_Stop(server);
	ggmsg_Destory(server);
	ggmsg_Stop(client);
	ggmsg_Destory(client);
}

void TestFlashClient() 
{
	auto server = ggmsg_Create();
	ggmsg_Start(server, 9, 9009, OnPassiveConnect, OnPassiveDisConnect, OnServerReceiveMsg);

	for (int i=0;i<10000;++i)
	{
		auto client = ggmsg_Create();
		ggmsg_Connect(server, "127.0.0.1", 9009, OnPositiveConnect, OnPositiveDisConnect, OnReceiveMsg);
		char msg[] = "hello server";
		int nMsgLen = strlen(msg) + 1;
		ggmsg_SendToService(client, 9, msg, nMsgLen);
		ggmsg_Stop(client);
		ggmsg_Destory(client);
	}

	ggmsg_Stop(server);
	ggmsg_Destory(server);
}

int main(int argc, char *argv[])
{	
	TestFlashClient();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

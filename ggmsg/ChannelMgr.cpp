#include "stdafx.h"

#include "ChannelMgr.h"
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

void ChannelMgr::Start(int nServiceID, short port, DataHandler handler)
{
	m_nServiceID = nServiceID;
	m_handler = handler;
	working_ = true;
	acceptor_ = new tcp::acceptor(m_ioContext, tcp::endpoint(tcp::v4(), port));
	
	do_accept();
	io_thread_ = std::thread([this]()
	{
		// ѭ���ſ����÷��񲻼��
		// ����ͻ��˶Ͽ����Ӿ��п�����io_context.run();�ܳ��쳣
		while (working_) {
			//try
			//{
				m_ioContext.run();
			//}
			//catch (std::exception& e)
			//{
			//	std::cerr << "Exception: " << e.what() << "\n";
			//	TRACE("exception: %s\n", e.what());
			//}
		}
	});
}

void ChannelMgr::Stop()
{
	if( working_ )
	{
		working_ = false;
		m_ioContext.stop();
		io_thread_.join();
	}
	
}

void ChannelMgr::GetChannelList(std::vector<std::shared_ptr<Channel> > &Channel_list)
{
	std::lock_guard<std::mutex> lk(Channels_lock_);
	for(auto & s : Channels_) {
		Channel_list.push_back(s.second);
	}
}

bool ChannelMgr::Send(int nServiceID, const void *pData, int nDataLen)
{
	std::shared_ptr<Channel> s;
	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		auto it = m_services.find(nServiceID);
		if (it == m_services.end()) {
			return false;
		}

		s = it->second;
	}
	
	s->SendMsg(pData, nDataLen);
	return true;
}

bool ChannelMgr::SendToAll(const void *pData, int nDataLen)
{

	{
		std::lock_guard<std::mutex> lk(Channels_lock_);
		for (auto & s : m_services)
		{
			s.second->SendMsg(pData, nDataLen);
		}
	}

	return true;
}

void ChannelMgr::do_accept()
{
	acceptor_->async_accept(
		[this](boost::system::error_code ec, tcp::socket socket)
	{
		if (!ec)
		{
			std::stringstream ip_port;
			std::string ip = socket.remote_endpoint().address().to_string();
			short port = socket.remote_endpoint().port();
			ip_port << socket.remote_endpoint().address().to_string() << ":" << socket.remote_endpoint().port();

			auto s = std::make_shared<Channel>(this, std::move(socket), &m_ioContext, Channel::passive);
			s->Start();
		}

		do_accept();
	});
}

bool ChannelMgr::Connect(const std::string & strHost, short sPort)
{
	tcp::resolver resolver(m_ioContext);
	auto endpoints = resolver.resolve(strHost, std::to_string(sPort));
	auto pConnectSocket = new tcp::socket(m_ioContext);
	auto pEndPoint = new tcp::resolver::results_type();
	*pEndPoint = endpoints;
	auto pTimer = new time_t_timer(m_ioContext);

	DoConnect(pConnectSocket, pEndPoint, pTimer);
	
	return true;
}

void ChannelMgr::DoConnect(tcp::socket *pConnectSocket, tcp::resolver::results_type *pEndPoint, time_t_timer *pTimer)
{
	boost::asio::async_connect(*pConnectSocket, *pEndPoint,
		[this, pConnectSocket, pEndPoint, pTimer](boost::system::error_code ec, tcp::endpoint) {
		if (ec) {
			// �ٴ�����
			// ��Ȼ������ԶԴ�������жϣ��ǿ�������������
			pTimer->expires_from_now(5);
			pTimer->async_wait([this, pConnectSocket, pEndPoint, pTimer](const boost::system::error_code& ec) {
				DoConnect(pConnectSocket, pEndPoint, pTimer);
			});
		}
		else {
			auto s = std::make_shared<Channel>(this, std::move(*pConnectSocket), &m_ioContext, Channel::positive);
			s->DoReqShakeHand();
			delete pConnectSocket;
			delete pEndPoint;
			delete pTimer;
		}
	});
}

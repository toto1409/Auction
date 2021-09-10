/*
 * echo_selserv_win.c
 * Written by SW. YOON
 */
#include "ClientInfo.h"
#include "AuctionInfo.h"
#include "AuctionGroupInfo.h"


SOCKET hDummySock;
HANDLE hEvent;
CRITICAL_SECTION cs;

CLinkedList<_ClientInfo*>* User_List;
CLinkedList<_User_Info*>* Join_List;
CLinkedList<_AuctionInfo*>* Auction_List;
CLinkedList<_AuctionGroupInfo*>* Auction_Group_List;

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


void GetProtocol(char* _ptr, PROTOCOL& _protocol)
{
	memcpy(&_protocol, _ptr, sizeof(PROTOCOL));

}

void PackPacket(char* _buf, PROTOCOL _protocol, int& _size)
{
	char* ptr = _buf;
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void PackPacket(char* _buf, PROTOCOL _protocol, int _data, int& _size)
{
	char* ptr = _buf;
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	_size = _size + sizeof(_data);

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void PackPacket(char* _buf, PROTOCOL _protocol, int _data, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	_size = _size + sizeof(_data);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void PackPacket(char* _buf, PROTOCOL _protocol, CLinkedList<_AuctionInfo*>* _list, int& _size)
{
	char* ptr = _buf;	
	_size = 0;
	int count = _list->GetCount();

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	char* countptr = ptr;
	ptr = ptr + sizeof(count);
	_size = _size + sizeof(count);

	_list->SearchStart();

	while(1)
	{
		_AuctionInfo* info = _list->SearchData();
		if (info == nullptr)
		{
			break;
		}

		if (info->auction_state == AUCTION_COMPLETE)
		{
			count--;
			continue;
		}

		memcpy(ptr, &info->auction_product_code, sizeof(info->auction_product_code));
		ptr = ptr + sizeof(info->auction_product_code);
		_size = _size + sizeof(info->auction_product_code);

		
		int namesize = strlen(info->auction_product);
		memcpy(ptr, &namesize, sizeof(namesize));
		ptr = ptr + sizeof(namesize);
		_size = _size + sizeof(namesize);

		memcpy(ptr, info->auction_product, namesize);
		ptr = ptr + namesize;
		_size = _size + namesize;
				
		memcpy(ptr, &info->auction_price, sizeof(info->auction_price));
		ptr = ptr + sizeof(info->auction_price);
		_size = _size + sizeof(info->auction_price);
	}

	_list->SearchEnd();

	memcpy(countptr, &count, sizeof(count));

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void PackPacket(char* _buf, PROTOCOL _protocol, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);	

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void UnPackPacket(char* _buf, char* _str1, char* _str2, char* _str3, int& _data)
{
	int str1size, str2size, str3size;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;

	memcpy(&str3size, ptr, sizeof(str3size));
	ptr = ptr + sizeof(str3size);

	memcpy(_str3, ptr, str3size);
	ptr = ptr + str3size;

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);
}

void UnPackPacket(char* _buf, char* _str1, char* _str2)
{
	int str1size, str2size;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;
}

void UnPackPacket(char* _buf, int& _data1)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data1, ptr, sizeof(_data1));
	ptr = ptr + sizeof(_data1);

}

void UnPackPacket(char* _buf, int& _data1, int& _data2)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data1, ptr, sizeof(_data1));
	ptr = ptr + sizeof(_data1);

	memcpy(&_data2, ptr, sizeof(_data2));
	ptr = ptr + sizeof(_data2);
}

void UnPackPacket(char* _buf, _AuctionInfo& _info)
{
	int strsize;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_info.auction_product, ptr, strsize);
	ptr = ptr + strsize;	

	memcpy(&_info.auction_user_count, ptr, sizeof(_info.auction_user_count));
	ptr = ptr + sizeof(_info.auction_user_count);
}

BOOL SearchFile(char *filename)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(filename, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;
	else {
		FindClose(hFindFile);
		return TRUE;
	}
}

bool FileDataLoad()
{
	if (!SearchFile("userinfo.info"))
	{
		FILE* fp = fopen("userinfo.info", "wb");
		fclose(fp);
		return true;
	}

	FILE* fp = fopen("userinfo.info", "rb");
	if (fp == NULL)
	{
		return false;
	}

	_User_Info info;
	memset(&info, 0, sizeof(_User_Info));

	while (1)
	{
		fread(&info, sizeof(_User_Info), 1, fp);
		if (feof(fp))
		{
			break;
		}
		_User_Info* ptr = new _User_Info;
		memcpy(ptr, &info, sizeof(_User_Info));
		Join_List->Insert(ptr);
	}

	fclose(fp);
	return true;
}

bool FileDataAdd(_User_Info* _info)
{
	FILE* fp = fopen("userinfo.info", "ab");
	if (fp == NULL)
	{
		return false;
	}

	int retval = fwrite(_info, 1, sizeof(_User_Info), fp);

	if (retval != sizeof(_User_Info))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool FileDataSave()
{
	FILE* fp = fopen("userinfo.info", "wb");
	if (fp == NULL)
	{
		return false;
	}

	Join_List->SearchStart();

	while (1)
	{
		_User_Info* info = Join_List->SearchData();
		if (info == nullptr)
		{
			break;
		}
		int retval = fwrite(info, 1, sizeof(_User_Info), fp);

		if (retval != sizeof(_User_Info))
		{
			fclose(fp);
			return false;
		}		
	}

	Join_List->SearchEnd();

	fclose(fp);
	return true;
}

int MessageRecv(_ClientInfo* _info)
{
	int retval = recv(_info->sock, _info->recv_buf + _info->comp_recvbytes, _info->recvbytes - _info->comp_recvbytes, 0);
	if (retval == SOCKET_ERROR) //�������������û�� ���
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		return DISCONNECTED;
	}
	else
	{
		_info->comp_recvbytes += retval;
		if (_info->comp_recvbytes == _info->recvbytes)
		{
			_info->comp_recvbytes = 0;
			_info->recvbytes = 0;
			return SOC_TRUE;
		}
		return SOC_FALSE;
	}

}

int MessageSend(_ClientInfo* _info)
{
	int retval = send(_info->sock, _info->send_buf + _info->comp_sendbytes,
		_info->sendbytes - _info->comp_sendbytes, 0);
	if (retval == SOCKET_ERROR)
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		DISCONNECTED;
	}
	else
	{
		_info->comp_sendbytes = _info->comp_sendbytes + retval;

		if (_info->sendbytes == _info->comp_sendbytes)
		{
			_info->sendbytes = 0;
			_info->comp_sendbytes = 0;

			return SOC_TRUE;
		}
		else
		{
			return SOC_FALSE;
		}
	}
}

int PacketRecv(_ClientInfo* _ptr)
{
	if (!_ptr->r_sizeflag)
	{
		_ptr->recvbytes = sizeof(int);
		int retval = MessageRecv(_ptr);
		switch (retval)
		{
		case SOC_TRUE:
			memcpy(&_ptr->recvbytes, _ptr->recv_buf, sizeof(int));
			_ptr->r_sizeflag = true;
			return SOC_FALSE;
		case SOC_FALSE:
			return SOC_FALSE;
		case ERROR_DISCONNECTED:
			err_display("recv error()");
			return DISCONNECTED;
		case DISCONNECTED:
			return DISCONNECTED;
		}
	}

	int retval = MessageRecv(_ptr);
	switch (retval)
	{
	case SOC_TRUE:
		_ptr->r_sizeflag = false;
		return SOC_TRUE;
	case SOC_FALSE:
		return SOC_FALSE;
	case ERROR_DISCONNECTED:
		err_display("recv error()");
		return DISCONNECTED;
	case DISCONNECTED:
		return DISCONNECTED;
	}
}

_ClientInfo* AddClient(SOCKET _sock, SOCKADDR_IN _clientaddr)
{
	EnterCriticalSection(&cs);
	printf("\nClient ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(_clientaddr.sin_addr),
		ntohs(_clientaddr.sin_port));

	_ClientInfo* ptr = new _ClientInfo();	
	ptr->sock = _sock;
	memcpy(&ptr->addr, &_clientaddr, sizeof(SOCKADDR_IN));	
	User_List->Insert(ptr);
	LeaveCriticalSection(&cs);
	return ptr;
}

void RemoveClient(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	closesocket(_ptr->sock);

	printf("\nClient ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",inet_ntoa(_ptr->addr.sin_addr),ntohs(_ptr->addr.sin_port));	

	User_List->Delete(_ptr);	
	delete _ptr;
	LeaveCriticalSection(&cs);
}

_AuctionInfo* AddAuctionInfo(const char* _product, int _count, int _price)
{
	EnterCriticalSection(&cs);
	static int auction_user_count = 0;
	_AuctionInfo* info = new _AuctionInfo;	

	strcpy(info->auction_product, _product);
	info->auction_user_count = _count;
	info->auction_price = _price;
	info->auction_product_code = auction_user_count++;
	info->auction_state = AUCTION_ONGOING;
	Auction_List->Insert(info);
	LeaveCriticalSection(&cs);
	return info;
}

void RemoveAuctionInfo(_AuctionInfo* _auctioninfo)
{	
	EnterCriticalSection(&cs);
	Auction_List->Delete(_auctioninfo);
	delete _auctioninfo;
	LeaveCriticalSection(&cs);
}

_ClientInfo* SearchClientInfo(const char* _id)
{
	_ClientInfo* info = nullptr;

	User_List->SearchStart();

	while (1)
	{
		info = User_List->SearchData();
		if (info == nullptr)
		{			
			break;
		}
		if (!strcmp(info->userinfo->id, _id))
		{			
			break;
		}
	}

	User_List->SearchEnd();

	return info;
}

_AuctionInfo* SearchAuctionInfo(int _auction_code)
{
	_AuctionInfo* auction_info = nullptr;
	Auction_List->SearchStart();

	while (1)
	{
		auction_info = Auction_List->SearchData();
		if (auction_info == nullptr)
		{			
			break;
		}

		if (auction_info->auction_state == AUCTION_COMPLETE)
		{
			continue;
		}

		if (auction_info->auction_product_code == _auction_code)
		{			
			break;
		}
	}

	Auction_List->SearchEnd();
	return auction_info;
}

_AuctionGroupInfo* SearchAuctionGroupInfo(_AuctionInfo* _auction_info)
{
	_AuctionGroupInfo* group_info = nullptr;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info->auction_info == _auction_info)
		{			
			break;
		}
	}

	Auction_Group_List->SearchEnd();

	return group_info;
}

_AuctionGroupInfo* AddAuctionGroupInfo(_AuctionInfo* _info)
{	
	EnterCriticalSection(&cs);
	_AuctionGroupInfo* group_info=nullptr;
	
	bool flag = false;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info->auction_info==_info)
		{
			if (group_info->isfull())
			{
				Auction_Group_List->SearchEnd();
				LeaveCriticalSection(&cs);
				return nullptr;
			}
			
			flag = true;
			break;
		}
	}

	Auction_Group_List->SearchEnd();

	if (!flag)
	{		
		group_info = new _AuctionGroupInfo(_info);

		Auction_Group_List->Insert(group_info);
	}		
	LeaveCriticalSection(&cs);
	return group_info;
}

void RemoveAuctionGroupInfo(_AuctionGroupInfo* _group_info)
{
	EnterCriticalSection(&cs);
	_AuctionGroupInfo* group_info = nullptr;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info == _group_info)
		{		
			Auction_Group_List->Delete(_group_info);
			delete _group_info;
			break;
		}
	}

	Auction_Group_List->SearchEnd();
	LeaveCriticalSection(&cs);
}

bool AllAuctionComplete()
{
	Auction_List->SearchStart();
	
	while (1)
	{
		_AuctionInfo* auction_info = Auction_List->SearchData();		
		if (auction_info == nullptr)
		{
			break;
		}

		if (auction_info->auction_state != AUCTION_COMPLETE)
		{
			return false;
		}
	}

	return true;
}

bool CheckAuctionCompleteGroup(_ClientInfo* _info)
{
	_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->try_auction->info);

	group_info->userlist_delete(_info);

	if (group_info->get_user_count() == 0)
	{
		Auction_Group_List->Delete(group_info);
		return true;
	}   	

	return false;
}

void ExitAuctionGroup(_ClientInfo* _info)
{
	if (_info->try_auction->info != nullptr && _info->try_auction->info->auction_state!=AUCTION_COMPLETE)
	{
		_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->try_auction->info);
		
		group_info->userlist_delete(_info);
		
		if (group_info->GetMaxBidUser() == _info)
		{
			group_info->clear_timer_count();

			group_info->ChangeMaxBidUser();

			group_info->searchstart();
			while (1)
			{
				_ClientInfo* client_info;
				if (!group_info->searchdata(client_info))
				{
					break;
				}

				client_info->pre_state = client_info->state;
				client_info->state = USER_EXIT_INFO_SEND_STAE;
			}

			group_info->searchend();

		}
	}

}

bool TryBid(_ClientInfo* _info, int _price)
{
	_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->try_auction->info);
	
	_ClientInfo* user = group_info->GetMaxBidUser();
		   	
	if (user == nullptr)
	{
		if (group_info->auction_info->auction_price > _price)
		{
			return false;
		}

		_info->try_auction->try_price = _price;
		group_info->UpdateMaxBidUser(_info);
		return true;
	}

	if (user->try_auction->try_price > _price)
	{
		return false;
	}

	_info->try_auction->try_price = _price;
	group_info->UpdateMaxBidUser(_info);
	return true;
}

DWORD CALLBACK CountThread(LPVOID ptr)
{	
	char msg[BUFSIZE] = { 0 };
	SOCKADDR_IN dumyAddr;

	memset(&dumyAddr, 0, sizeof(dumyAddr));

	dumyAddr.sin_family = AF_INET;
	dumyAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dumyAddr.sin_port = htons(9900);

	while (1)
	{
		int result = WaitForSingleObject(hEvent, 100);
		EnterCriticalSection(&cs);

		switch (result)
		{
		case WAIT_OBJECT_0:
			LeaveCriticalSection(&cs);
			continue;
		case WAIT_TIMEOUT:

			Auction_Group_List->SearchStart();

			while (1)
			{
			
				_AuctionGroupInfo* group_info = Auction_Group_List->SearchData();
				if (group_info == nullptr)
				{					
					break;
				}
				
				if (group_info->GetMaxBidUser() == nullptr)
				{					
					continue;
				}

				group_info->increase_timer_count();
				if (group_info->check_timer_count())
				{
					group_info->searchstart();
					
					while (1)
					{
						_ClientInfo* client_info = nullptr;
						if (!group_info->searchdata(client_info))
						{							
							break;
						}
						
						
						client_info->pre_state = client_info->state;
						client_info->state = AUCTION_COUNT_SEND_STATE;
					}
					group_info->searchend();

					sendto(hDummySock, msg, sizeof(int), 0, (SOCKADDR*)&dumyAddr, sizeof(dumyAddr));
				}
				
			}

			Auction_Group_List->SearchEnd();
		}

		LeaveCriticalSection(&cs);


	}
}



int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr, dumyAddr;
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;
	int retval;
	FD_SET Rset, Wset;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		err_quit("WSAStartup() error!");
	
	InitializeCriticalSection(&cs);

	hEvent = CreateEvent(nullptr, false, false, nullptr);

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		err_quit("socket() error");

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(9000);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		err_quit("bind() error");	

	hDummySock = socket(PF_INET, SOCK_DGRAM, 0);
	if (hDummySock == INVALID_SOCKET)
		err_quit("socket() error");

	dumyAddr.sin_family = AF_INET;
	dumyAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dumyAddr.sin_port = htons(9900);

	if (bind(hDummySock, (SOCKADDR*)&dumyAddr, sizeof(dumyAddr)) == SOCKET_ERROR)
		err_quit("bind() error");


	if (listen(hServSock, 5) == SOCKET_ERROR)
		err_quit("listen() error");

	User_List = new CLinkedList<_ClientInfo*>();
	Join_List = new CLinkedList<_User_Info*>();
	Auction_List = new CLinkedList<_AuctionInfo*>();
	Auction_Group_List = new CLinkedList<_AuctionGroupInfo*>();

	if (!FileDataLoad())
	{
		err_quit("file read error!");
	}	

	char msg[BUFSIZE];
	PROTOCOL protocol;
	int size;

	AddAuctionInfo("��Ź", 3, 100000);
	AddAuctionInfo("ħ��", 2, 100000);
	AddAuctionInfo("�����", 3, 100000);
	AddAuctionInfo("å��", 3, 100000);
	AddAuctionInfo("����", 3, 100000);

	CreateThread(NULL, 0, CountThread, NULL, 0, NULL);

	while (1)
	{
		FD_ZERO(&Rset);
		FD_ZERO(&Wset);

		FD_SET(hServSock, &Rset);
		FD_SET(hDummySock, &Rset);

		User_List->SearchStart();

		while(1)
		{
			EnterCriticalSection(&cs);

			_ClientInfo* ptr = User_List->SearchData();
			if (ptr == nullptr)
			{
				LeaveCriticalSection(&cs);
				break;
			}
			
			FD_SET(ptr->sock, &Rset);

			switch (ptr->state)
			{
			case JOIN_RESULT_SEND_STATE:
			case LOGIN_RESULT_SEND_STATE:
			case USER_INFO_SEND_STATE:
			case AUCION_MONEY_UPDATE_SEND_STATE:
			case LOGOUT_RESULT_SEND_STATE:		
			case USER_EXIT_INFO_SEND_STAE:
			case AUCTION_LIST_SEND_STATE:
			case AUCTION_BID_WAIT_SEND_STATE:
			case AUCTION_BID_START_SEND_STATE:
			case AUCTION_BID_SEND_STATE:
			case AUCTION_COUNT_SEND_STATE:
			case AUCTION_COMPLETE_STATE:
			case CONNECT_END_SEND_STATE:
			case AUCTION_ERROR_SEND_STATE:
				FD_SET(ptr->sock, &Wset);
				break;
			}	

			LeaveCriticalSection(&cs);
		}

		User_List->SearchEnd();

		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR)
		{
			err_quit("select() error");
		}
			

		if (FD_ISSET(hServSock, &Rset)) 
		{
			int clntLen = sizeof(clntAddr);
			hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntLen);
			_ClientInfo* ptr = AddClient(hClntSock, clntAddr);
			ptr->state = LOGIN_MENU_SELECT_STATE;
			continue;
		}		

		User_List->SearchStart();

		while(1)
		{
			_ClientInfo* ptr = User_List->SearchData();

			if (ptr == nullptr)
			{
				break;
			}

			if (FD_ISSET(ptr->sock, &Rset))
			{				
				int result = PacketRecv(ptr);

				switch (result)
				{
				case DISCONNECTED:
					ptr->state = DISCONNECTED_STATE;
					break;
				case SOC_FALSE:
					continue;
				case SOC_TRUE:
					break;
				}

				GetProtocol(ptr->recv_buf, protocol);

				if (ptr->state !=DISCONNECTED_STATE && protocol == CONNECT_END)
				{
					EnterCriticalSection(&cs);
					ptr->pre_state = ptr->state;
					ptr->state = CONNECT_END_SEND_STATE;
					LeaveCriticalSection(&cs);
					continue;
				}

				switch (ptr->state)
				{
				case LOGIN_MENU_SELECT_STATE:		
					EnterCriticalSection(&cs);
					
					switch (protocol)
					{
					case JOIN_INFO:
						memset(ptr->temp_user, 0, sizeof(_User_Info));
						UnPackPacket(ptr->recv_buf, ptr->temp_user->id, ptr->temp_user->pw, ptr->temp_user->nickname, ptr->temp_user->auction_money);
						ptr->state = JOIN_RESULT_SEND_STATE;
						break;
					case LOGIN_INFO:
						memset(ptr->temp_user, 0, sizeof(_User_Info));
						UnPackPacket(ptr->recv_buf, ptr->temp_user->id, ptr->temp_user->pw);
						ptr->state = LOGIN_RESULT_SEND_STATE;
						break;
					}
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_MENU_SELECT_STATE:				
					EnterCriticalSection(&cs);
					switch (protocol)
					{
					
					case REQ_AUCTION_LIST_INFO:
						ptr->state=AUCTION_LIST_SEND_STATE;
						break;
					case AUCTION_SELECT_INFO:
						{
							int code = 0;							
							UnPackPacket(ptr->recv_buf, code);
							
							_AuctionInfo* auc_info = SearchAuctionInfo(code);
							if (auc_info == nullptr)
							{							
								ptr->try_auction->result = AUCTION_CODE_ERROR;
								ptr->pre_state = ptr->state;
								ptr->state = AUCTION_ERROR_SEND_STATE;
								break;
							}					

							_AuctionGroupInfo* group_info = AddAuctionGroupInfo(auc_info);

							if (group_info == nullptr)
							{
								ptr->try_auction->result = AUCTION_GROUP_FULL_ERROR;
								ptr->pre_state = ptr->state;
								ptr->state = AUCTION_ERROR_SEND_STATE;
								break;
							}

							ptr->auction_start(auc_info);
							group_info->userlist_insert(ptr);
							ptr->state = AUCTION_BID_WAIT_SEND_STATE;
						}						
						break;						
					case DEPOSIT_INFO:
						{
							int price;
							UnPackPacket(ptr->recv_buf, price);
							ptr->deposit(price);

							ptr->pre_state = ptr->state;
							ptr->state = AUCION_MONEY_UPDATE_SEND_STATE;

						}
						break;						
					case LOGOUT:
						ptr->logout();
						ptr->state = LOGOUT_RESULT_SEND_STATE;
						break;
					}
					LeaveCriticalSection(&cs);
					break;
				
				case AUCTION_BID_STATE:
					EnterCriticalSection(&cs);
					{
						int price;						
						_AuctionGroupInfo* group_info;

						switch (protocol)
						{
						case AUCTION_BID_PRICE:
							UnPackPacket(ptr->recv_buf, price);

							if (!ptr->check_money(price))
							{
								ptr->try_auction->result = AUCTION_SHORT_OF_MONEY_ERROR;
								ptr->pre_state = ptr->state;
								ptr->state = AUCTION_ERROR_SEND_STATE;
								break;
							}

							if (!TryBid(ptr, price))
							{
								ptr->try_auction->result = AUCTION_BID_MONEY_ERROR;
								ptr->pre_state = ptr->state;
								ptr->state = AUCTION_ERROR_SEND_STATE;
								break;
							}

							group_info = SearchAuctionGroupInfo(ptr->try_auction->info);
							group_info->searchstart();

							while (1)
							{
								_ClientInfo* client_info = nullptr;
								if (!group_info->searchdata(client_info))
								{
									break;
								}

								client_info->pre_state = client_info->state;
								client_info->state = AUCTION_BID_SEND_STATE;
							}

							group_info->searchend();
							group_info->clear_timer_count();
							break;
						case LOGOUT:
							ptr->logout();
							ptr->state = LOGOUT_RESULT_SEND_STATE;
							break;
						}

					}
					LeaveCriticalSection(&cs);
					break;			

				case DISCONNECTED_STATE:
					EnterCriticalSection(&cs);
					ExitAuctionGroup(ptr);
					RemoveClient(ptr);
					FileDataSave();
					LeaveCriticalSection(&cs);
					continue;					
				}
			}					

			if (FD_ISSET(ptr->sock, &Wset))
			{
				int join_result = NODATA;
				int login_result = NODATA;		
				int result;
				
				switch (ptr->state)
				{
				case JOIN_RESULT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						Join_List->SearchStart();

						while (1)
						{
							_User_Info* user_info = Join_List->SearchData();
							if (user_info == nullptr)
							{
								break;
							}
							if (!strcmp(user_info->id, ptr->temp_user->id))
							{
								join_result = ID_EXIST;
								strcpy(msg, ID_EXIST_MSG);
								break;
							}
						}

						Join_List->SearchEnd();

						if (join_result == NODATA)
						{
							_User_Info* user = new _User_Info;
							memset(user, 0, sizeof(_User_Info));
							strcpy(user->id, ptr->temp_user->id);
							strcpy(user->pw, ptr->temp_user->pw);
							strcpy(user->nickname, ptr->temp_user->nickname);
							user->auction_money = ptr->temp_user->auction_money;

							FileDataAdd(user);

							Join_List->Insert(user);
							join_result = JOIN_SUCCESS;
							strcpy(msg, JOIN_SUCCESS_MSG);
						}

						protocol = JOIN_RESULT;

						PackPacket(ptr->send_buf, protocol, join_result, msg, size);
						ptr->sendbytes=size;
					}					

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = LOGIN_MENU_SELECT_STATE;
					LeaveCriticalSection(&cs);
					break;
				case LOGIN_RESULT_SEND_STATE:	
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{						
						Join_List->SearchStart();
						while (1)
						{
							_User_Info* user_info = Join_List->SearchData();
							if (user_info == nullptr)
							{
								break;
							}
							if (!strcmp(user_info->id, ptr->temp_user->id))
							{
								if (!strcmp(user_info->pw, ptr->temp_user->pw))
								{
									ptr->login(user_info);
									login_result = LOGIN_SUCCESS;
									strcpy(msg, LOGIN_SUCCESS_MSG);
								}
								else
								{
									login_result = PW_ERROR;
									strcpy(msg, PW_ERROR_MSG);
								}
								break;
							}
						}

						Join_List->SearchEnd();
						if (login_result == NODATA)
						{
							login_result = ID_ERROR;
							strcpy(msg, ID_ERROR_MSG);
						}

						protocol = LOGIN_RESULT;

						PackPacket(ptr->send_buf, protocol, login_result, msg, size);
						ptr->sendbytes=size;
					}					

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					if (login_result == LOGIN_SUCCESS)
					{
						ptr->state = USER_INFO_SEND_STATE;						
					}
					else
					{
						ptr->state = LOGIN_MENU_SELECT_STATE;
					}
					LeaveCriticalSection(&cs);
					break;	
				case USER_INFO_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, USER_INFO,  ptr->userinfo->auction_money, ptr->userinfo->nickname, size);
						ptr->sendbytes = size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = AUCTION_MENU_SELECT_STATE;
					LeaveCriticalSection(&cs);
					break;
				case AUCION_MONEY_UPDATE_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, AUCTION_MONEY_UPDATE_INFO, ptr->userinfo->auction_money , size);
						ptr->sendbytes = size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					ptr->state = ptr->pre_state;
					LeaveCriticalSection(&cs);
					break;
				
				case AUCTION_LIST_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						if (AllAuctionComplete())
						{
							PackPacket(ptr->send_buf, ALL_AUCTION_COMPLETE, AUCTION_ALL_AUCTION_COMPLETE_MSG, size);
						}
						else
						{
							PackPacket(ptr->send_buf, AUCTION_LIST_INFO, Auction_List, size);
						}
						
						ptr->sendbytes=size;
					}
					
					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = AUCTION_MENU_SELECT_STATE;
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_BID_WAIT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, AUCTION_BID_WAIT, AUCTION_BID_WAIT_MSG, size);
						ptr->sendbytes=size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = AUCTION_BID_WAIT_STATE;			
					LeaveCriticalSection(&cs);
				case AUCTION_BID_WAIT_STATE:
					EnterCriticalSection(&cs);
					{
						_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(ptr->try_auction->info);
						if (group_info->isfull())
						{
							group_info->searchstart();
							while (1)
							{
								_ClientInfo* client_info = nullptr;
								if(!group_info->searchdata(client_info))							
								{
									break;
								}
								client_info->state = AUCTION_BID_START_SEND_STATE;
							}
							group_info->searchend();

						}

					}
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_BID_START_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, AUCTION_BID_START, AUCTION_BID_START_MSG, size);
						ptr->sendbytes=size;
					}
				
					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					ptr->state = AUCTION_BID_STATE;
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_BID_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(ptr->try_auction->info);
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "%s���� %d���� �����ϼ̽��ϴ�.\n", group_info->GetMaxBidUser()->userinfo->nickname,
							group_info->GetMaxBidUser()->try_auction->try_price);
						PackPacket(ptr->send_buf, AUCTION_BID_PRICE, msg, size);
						ptr->sendbytes=size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = ptr->pre_state;
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_COUNT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(ptr->try_auction->info);
						sprintf(msg, "%d�� �������ϴ�. 5�ʰ� ������ ��Ű� ����˴ϴ�.\n", group_info->get_timer_count());
						PackPacket(ptr->send_buf, AUCTION_SECOND_COUNT, msg, size);
						ptr->sendbytes=size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					{
						_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(ptr->try_auction->info);

						if (group_info->get_timer_count() >= MAX_AUCTION_COUNT)
						{						
							if (group_info->GetMaxBidUser() == ptr)
							{
								ptr->try_auction_success();

								ptr->try_auction->info->AuctionComplete(ptr->userinfo, ptr->try_auction->try_price);
							}
							ptr->state = AUCTION_COMPLETE_STATE;
							LeaveCriticalSection(&cs);
							break;
						}
					}
					ptr->state = ptr->pre_state;										
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_COMPLETE_STATE:	
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						if (ptr->check_auction_success())
						{
							sprintf(msg, "%s��ǰ�� ���� ��ſ� �����߽��ϴ�. �������� %d���Դϴ�.\n",
								ptr->try_auction->info->auction_product, ptr->try_auction->info->auction_price);
						}
						else
						{
							sprintf(msg, "%s��ǰ�� ���� ��ſ� �����߽��ϴ�. %s�Կ��� �����Ǿ����� �������� %d���Դϴ�.\n",
								ptr->try_auction->info->auction_product,
								ptr->try_auction->info->max_bid_user->nickname,
								ptr->try_auction->info->auction_price);
						}

						PackPacket(ptr->send_buf, AUCTION_RESULT_INFO, msg, size);
						ptr->sendbytes=size;
					}
									
					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					if (CheckAuctionCompleteGroup(ptr))
					{
						//
					}

					if (ptr->try_auction->result == AUCTION_SUCCESS)
					{
						ptr->auction_end();
						ptr->pre_state = AUCTION_MENU_SELECT_STATE;
						ptr->state = AUCION_MONEY_UPDATE_SEND_STATE;
					}
					else
					{
						ptr->auction_end();						
						ptr->state = AUCTION_MENU_SELECT_STATE;
					}
					
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_ERROR_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						memset(msg, 0, sizeof(msg));

						switch (ptr->try_auction->result)
						{
						case AUCTION_GROUP_FULL_ERROR:
							strcpy(msg, AUCTION_FULL_ERROR_MSG);
							break;
						case AUCTION_CODE_ERROR:
							strcpy(msg, AUCTION_CODE_ERROR_MSG);
							break;
						case AUCTION_BID_MONEY_ERROR:
							strcpy(msg, AUCTION_BID_MONEY_ERROR_MSG);
							break;
						case AUCTION_SHORT_OF_MONEY_ERROR:
							strcpy(msg, AUCTION_MONEY_ERROR_MSG);
							break;
						}
						PackPacket(ptr->send_buf, AUCTION_ERROR_INFO, msg, size);
						ptr->sendbytes=size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					ptr->state = ptr->pre_state;
					LeaveCriticalSection(&cs);
					break;
				case CONNECT_END_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, CONNECT_END, size);
						ptr->sendbytes = size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = ptr->pre_state;
					LeaveCriticalSection(&cs);
					break;
				case USER_EXIT_INFO_SEND_STAE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, USER_EXIT_INFO,USER_EXIT_INFO_MSG, size);
						ptr->sendbytes = size;
					}

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->state = ptr->pre_state;
					LeaveCriticalSection(&cs);
					break;
				case LOGOUT_RESULT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						PackPacket(ptr->send_buf, LOGOUT_RESULT, LOGOUT_MSG, size);
						ptr->sendbytes = size;
					}					

					result = MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->state = DISCONNECTED_STATE;
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ExitAuctionGroup(ptr);
					ptr->state = LOGIN_MENU_SELECT_STATE;
					LeaveCriticalSection(&cs);
					break;					
				}//switch protocol end

				if (ptr->state == DISCONNECTED_STATE)
				{
					RemoveClient(ptr);
					continue;
				}
			}//if wset end
		}// client service while end

		User_List->SearchEnd();
	}//accept while end

	closesocket(hServSock);

	DeleteCriticalSection(&cs);

	WSACleanup();
	return 0;
}




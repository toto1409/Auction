#pragma once
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "LinkedList.h"

#define BUFSIZE 4096
#define IDSIZE 255
#define PWSIZE 255
#define NICKNAMESIZE 255
#define PRODUCT_NAMESIZE 255
#define MAX_AUCTION_COUNT 5
#define MAX_BID_COUNT 100

#define ID_ERROR_MSG "���� ���̵��Դϴ�\n"
#define PW_ERROR_MSG "�н����尡 Ʋ�Ƚ��ϴ�.\n"
#define LOGIN_SUCCESS_MSG "�α��ο� �����߽��ϴ�.\n"
#define LOGOUT_MSG "�α׾ƿ��Ǿ����ϴ�.\n"
#define ID_EXIST_MSG "�̹� �ִ� ���̵� �Դϴ�.\n"
#define JOIN_SUCCESS_MSG "���Կ� �����߽��ϴ�.\n"
#define AUCTION_BID_WAIT_MSG "��ſ� �ο��� ���⸦ ��ٸ��ϴ�. ..\n"
#define AUCTION_ALL_AUCTION_COMPLETE_MSG "��� ��Ű� ����Ǿ����ϴ�...\n"
#define AUCTION_BID_START_MSG "��ſ� �ο��� á���ϴ�.. ������ �ּ���.\n"
#define AUCTION_FULL_ERROR_MSG "��� �ο����� ���� á���ϴ�. �ٸ� ��Ÿ� ������ �ּ���\n"
#define AUCTION_CODE_ERROR_MSG "��� �ڵ� �����Դϴ�.\n"
#define AUCTION_MONEY_ERROR_MSG "���� �ݾ��� �����մϴ�.\n"
#define AUCTION_BID_MONEY_ERROR_MSG "�ֱ� ���� ���ݺ��� ���� �ݾ��� �����ϼ̽��ϴ�.\n"
#define USER_EXIT_INFO_MSG "�ְ����� �����ߴ� ������ �����߽��ϴ�. �������ݺ��� �����մϴ�.\n"



enum STATE
{
	INIT_STATE = -1,
	LOGIN_MENU_SELECT_STATE = 1,
	JOIN_RESULT_SEND_STATE,
	LOGIN_RESULT_SEND_STATE,
	USER_INFO_SEND_STATE,
	LOGOUT_RESULT_SEND_STATE,

	AUCION_MONEY_UPDATE_SEND_STATE,

	AUCTION_MENU_SELECT_STATE,
	AUCTION_LIST_SEND_STATE,
	AUCTION_BID_WAIT_SEND_STATE,
	AUCTION_BID_WAIT_STATE,
	AUCTION_BID_START_SEND_STATE,
	AUCTION_BID_STATE,
	AUCTION_BID_SEND_STATE,
	AUCTION_COUNT_SEND_STATE,
	AUCTION_COMPLETE_STATE,
	AUCTION_ERROR_SEND_STATE,
	CONNECT_END_SEND_STATE,
	USER_EXIT_INFO_SEND_STAE,
	DISCONNECTED_STATE
};

enum
{
	ERROR_DISCONNECTED = -2,
	DISCONNECTED = -1,
	SOC_FALSE,
	SOC_TRUE
};


enum PROTOCOL
{
	JOIN_INFO,
	LOGIN_INFO,
	JOIN_RESULT,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	CONNECT_END,
	USER_EXIT_INFO,
	DEPOSIT_INFO,

	AUCTION_MONEY_UPDATE_INFO,
	USER_INFO,
	REQ_AUCTION_LIST_INFO,
	ALL_AUCTION_COMPLETE,
	AUCTION_LIST_INFO,
	AUCTION_BID_WAIT,
	AUCTION_BID_START,
	AUCTION_BID_PRICE,
	AUCTION_SECOND_COUNT,
	AUCTION_SELECT_INFO,
	AUCTION_RESULT_INFO,
	AUCTION_ERROR_INFO
};

enum
{
	NODATA = -1,
	ID_EXIST = 1,
	ID_ERROR,
	PW_ERROR,
	JOIN_SUCCESS,
	LOGIN_SUCCESS,

	AUCTION_SUCCESS,
	AUCTION_FAIL,
	AUCTION_ONGOING,
	AUCTION_COMPLETE,

	AUCTION_GROUP_FULL_ERROR,
	AUCTION_CODE_ERROR,
	AUCTION_BID_MONEY_ERROR,
	AUCTION_SHORT_OF_MONEY_ERROR
};

struct _User_Info
{
	char	id[IDSIZE];
	char	pw[PWSIZE];
	char	nickname[NICKNAMESIZE];
	int		auction_money;
};


/*
 * server_user.h
 *
 *  Created on: 13/11/2012
 *      Author: roni
 */

#ifndef SERVER_USER_H_
#define SERVER_USER_H_

#include "soapH.h"
#include <stdio.h>


#define DEBUG_MODE 0

#define DATA_PATH "server_data/"

#define MAXFRIENDS 100

struct Files{
	FILE *file;
	char *friend_nick;
};
typedef struct{
	char* nick;
	char* pass;
	int online;//0 - offline 1 - online

	int numFriends;
	int numSend;
	int numPending;

	struct Files *files[MAXFRIENDS];

	char* friends[MAXFRIENDS];//friends who accepted your friendship request
	char* friends_request_send[MAXFRIENDS];//friends that you've sent friendship request
	char* friends_request_pending[MAXFRIENDS];//friends who have sent friendship request to you

}User;


User* userInit(char* nick,char* pass);
void userFree(User* usr);

void setNick(User* usr,char* nick);
void setPass(User* usr,char* pass);

int addFriend(User* usr,char* friend_nick);
int removeFriend(User* usr,char* friend_nick);
int isFriend(User* usr,char *friend_nick);
int getFriends(User* usr,char* friends[MAXFRIENDS]);

int addFriendRequestSend(User* usr,char* friend_nick);
int removeFriendRequestSend(User* usr,char* friend_nick);
int isFriendRequestSend(User* usr,char* friend_nick);

int addFriendRequestPending(User* usr,char* friend_nick);
int removeFriendRequestPending(User* usr,char* friend_nick);
int isFriendRequestPending(User* usr,char* friend_nick);
int getFriendRequestsPending(User* usr,char* friends[MAXFRIENDS]);
int getFriendRequestPending(User* usr,char** friend_nick);

int copyToFile(FILE* file,char* friends[MAXFRIENDS],int num);
int isFileOpen(User* usr,char* friend_nick, int *pos);
int closeFiles(User* usr);
int readDownTo(User* usr,char* friend_nick,int num,char* result);

#endif /* SERVER_USER_H_ */

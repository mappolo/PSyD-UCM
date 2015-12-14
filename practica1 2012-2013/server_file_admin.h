/*
 * server_file_admin.h
 *
 *  Created on: 13/11/2012
 *      Author: roni
 */

#ifndef SERVER_FILE_ADMIN_H_
#define SERVER_FILE_ADMIN_H_

#include "server_user.h"
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

#define  MAXUSER 100
#define DEBUG_MODE 0

#define DATA_PATH "server_data/"

typedef struct{
	int numUser;
	User *listU[MAXUSER];
}LUser;

int serverInit(LUser* luser);
int addUsers(LUser* luser,char* nick,char* pass);
int removeUser(LUser* luser,char* nick);
User* getUser(LUser* luser,char* nick);
int userLogin(LUser* luser,char* nick,char* pass);
int userLogout(LUser* luser,char* nick,char* pass);

#endif /* SERVER_FILE_ADMIN_H_ */

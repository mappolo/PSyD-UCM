#ifndef server_user
#define server_user

#include <string.h>
#include "server_user.h"

//
// Create a new User
//
User* userInit(char* nick,char* pass){
	User *user = (User*)malloc(sizeof(User));
	user->nick = (char*)malloc(256*sizeof(char));
	strcpy (user->nick, nick);
	user->pass = (char*)malloc(256*sizeof(char));
	strcpy (user->pass, pass);

	/*user->nick = nick;
	user->pass = pass;*/

	int i;
	for(i = 0;i < MAXFRIENDS;i++){
		user->friends[i] = NULL;
		user->friends_request_pending[i] = NULL;
		user->friends_request_send[i] = NULL;

		user->files[i] = (struct Files*)malloc(sizeof(struct Files));
		user->files[i]->file = NULL;
		user->files[i]->friend_nick = NULL;
	}
	user->online = 0;
	user->numFriends = 0;
	user->numSend = 0;
	user->numPending = 0;

	return user;
}

//
// Free the memory
//
void userFree(User* usr){

	closeFiles(usr);

	int i;
	char* aux;
	if(DEBUG_MODE) printf("userFree -> Cerrando listas...\n");
	for(i = 0; i < MAXFRIENDS; i++)
	{
		aux = usr->friends[i];
		if(aux != NULL)
			free(aux);
		aux = usr->friends_request_pending[i];
		if(aux != NULL)
			free(aux);
		aux = usr->friends_request_send[i];
		if(aux != NULL)
			free(aux);
		free(usr->files[i]);

	}

	if(DEBUG_MODE) printf("userFree -> Liberando nick...\n");

	free(usr->nick);
	//*usr->nick = NULL;
	if(DEBUG_MODE) printf("userFree -> Liberando pass...\n");
	free(usr->pass);
	//*usr->pass = NULL;
	if(DEBUG_MODE) printf("userFree -> Liberando usuario...\n");

	free(usr);
	if(DEBUG_MODE) printf("userFree -> Usuario cerrado...\n");

}

//
//
//
void setNick(User* usr,char* nick){
	usr->nick = nick;
}

//
//
//
void setPass(User* usr,char* pass){
	usr->pass  = pass;
}

//
// Add a new friend to the friends list
//
int addFriend(User* usr,char* friend_nick){
	int i = 0;
	int j = -1;
	int found = 0;

	if(strcmp(usr->nick,friend_nick) == 0){
		return -1;
	}

	char* aux;
	while( i < MAXFRIENDS && found == 0){
		aux = usr->friends[i];
		if(aux != NULL){
			if(strcasecmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		else if(j == -1){
				j = i;
		}
		i++;
	}
	if(found == 0){
		usr->friends[j] = (char*)malloc(256*sizeof(char));
		strcpy(usr->friends[j] , friend_nick);
		usr->numFriends++;
		//printf("%s\n",usr->friends[j]);
	}
	return found;
}

//
//
//
int removeFriend(User* usr,char* friend_nick){
	int i = 0;
	int found = 0;
	char* aux;
	while(i < MAXFRIENDS && found == 0){
		aux = usr->friends[i];
		if(aux != NULL){
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		//if(found == 0)
			i++;
	}
	if(found == 1){
		free(usr->friends[i-1]);
		usr->friends[i-1] = NULL;
		usr->numFriends--;

		char *path = (char*)malloc(256*sizeof(char));

		sprintf(path,"rm %s%s/%s",DATA_PATH,usr->nick,friend_nick);
		system(path);
		free(path);
	}
	return found;
}

//
//
//
int isFriend(User* usr,char *friend_nick){
	int found = 0;
	int i = 0;
	char* aux;

	while(found == 0 && i < MAXFRIENDS){
		if(usr->friends[i] != NULL){
			aux = usr->friends[i];
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		i++;
	}
	return found;
}

//
//
//
int getFriends(User* usr,char* friends[MAXFRIENDS])
{
	if(usr->numFriends > 0)
	{
		char* aux;
		int i;

		for(i = 0; i < MAXFRIENDS; i++)
		{
			aux = usr->friends[i];
			if(aux != NULL)
			{
				friends[i] = (char*)malloc(256*sizeof(char));
				strcpy(friends[i],aux);
			}
		}
	}
	return 0;
}

//
// Add a new friend to the friends_request_send list
//
int addFriendRequestSend(User* usr,char* friend_nick)
{
	int i = 0;
	int j = -1;
	int found = 0;

	if(strcmp(usr->nick,friend_nick) == 0) return -1;

	char* aux;
	while( i < MAXFRIENDS && found == 0)
	{
		aux = usr->friends_request_send[i];
		if(aux != NULL)
		{
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}else if(j == -1)
		{
				j = i;
		}
		i++;
	}
	if(found == 0){
		usr->friends_request_send[j] = (char*)malloc(256*sizeof(char));
		strcpy(usr->friends_request_send[j] , friend_nick);
		usr->numSend++;
		//printf("%s\n",usr->friends[j]);
	}
	return found;
}

//
//
//
int removeFriendRequestSend(User* usr,char* friend_nick)
{
	int i = 0;
	int found = 0;
	char* aux;
	while(i < MAXFRIENDS && found == 0){
		aux = usr->friends_request_send[i];
		if(aux != NULL){
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		if(found == 0)
			i++;
	}
	if(found == 1){
		free(usr->friends_request_send[i]);
		usr->friends_request_send[i] = NULL;
		usr->numSend--;
		char* path = (char*)malloc(256*sizeof(char));
		sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,usr->nick,".send");
		if(DEBUG_MODE) printf("removeFriendshipRequestSend -> Borrando fichero enviados path: %s\n",path);
		system(path);
		sprintf(path,"%s%s/%s",DATA_PATH,usr->nick,".send");
		FILE* file;
		if(DEBUG_MODE) printf("removeFriendshipRequestSend -> Rescribiendo fichero enviados path: %s\n",path);

		if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

		copyToFile(file,usr->friends_request_send,usr->numSend);

		if(fclose(file) == -1) perror("Error cerrando fichero");
		free(path);
	}
	return found;
}

//
//
//
int isFriendRequestSend(User* usr,char* friend_nick)
{
	int found = 0;
	int i = 0;
	char* aux;

	while(found == 0 && i < MAXFRIENDS){
		if(usr->friends_request_send[i] != NULL){
			aux = usr->friends_request_send[i];
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		i++;
	}
	return found;
}

//
// Add a new friend to the friends_request_pending list
//
int addFriendRequestPending(User* usr,char* friend_nick)
{
	int i = 0;
	int j = -1;
	int found = 0;

	if(strcmp(usr->nick,friend_nick) == 0) return -1;

	char* aux;
	while( i < MAXFRIENDS && found == 0)
	{
		aux = usr->friends_request_pending[i];
		if(aux != NULL)
		{
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}else if(j == -1)
		{
				j = i;
		}
		i++;
	}
	if(found == 0){
		usr->friends_request_pending[j] = (char*)malloc(256*sizeof(char));
		strcpy(usr->friends_request_pending[j] , friend_nick);
		usr->numPending++;
		//printf("%s\n",usr->friends[j]);
	}
	return found;
}

//
//
//
int removeFriendRequestPending(User* usr,char* friend_nick)
{
	int i = 0;
	int found = 0;
	char* aux;
	while(i < MAXFRIENDS && found == 0){
		aux = usr->friends_request_pending[i];
		if(aux != NULL){
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		if(found == 0)
			i++;
	}
	if(found == 1){
		free(usr->friends_request_pending[i]);
		usr->friends_request_pending[i] = NULL;
		usr->numPending--;
		char* path = (char*)malloc(256*sizeof(char));
		sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,usr->nick,".pending");
		if(DEBUG_MODE) printf("removeFriendshipRequestSend -> Borrando fichero pendientes path: %s\n",path);
		FILE* file;

		sprintf(path,"%s%s/%s",DATA_PATH,usr->nick,".pending");
		if(DEBUG_MODE) printf("removeFriendshipRequestSend -> Rescribiendo fichero pendientes path: %s\n",path);

		if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

		copyToFile(file,usr->friends_request_pending,usr->numPending);

		if(fclose(file) == -1) perror("Error cerrando fichero");

		system(path);
		free(path);
	}
	return found;
}

//
//
//
int isFriendRequestPending(User* usr,char* friend_nick)
{
	int found = 0;
	int i = 0;
	char* aux;

	while(found == 0 && i < MAXFRIENDS){
		if(usr->friends[i] != NULL){
			aux = usr->friends_request_pending[i];
			if(strcmp(aux,friend_nick) == 0){
				found = 1;
			}
		}
		i++;
	}
	return found;
}

//
//
//
int getFriendRequestsPending(User* usr,char* friends[MAXFRIENDS])
{
	if(usr->numPending > 0)
	{
		char* aux;
		int i;

		for(i = 0; i < MAXFRIENDS; i++)
		{
			aux = usr->friends_request_pending[i];

			if(aux != NULL)
			{
				friends[i] = (char*)malloc(256*sizeof(char));
				strcpy(friends[i],aux);
			}
		}
	}
	return 0;
}

//
//
//
int getFriendRequestPending(User* usr,char** friend_nick)
{
	char* aux;
	int i = 0;
	int found = 0;
	while(found == 0 && i < MAXFRIENDS)
	{
		aux = usr->friends_request_pending[i];

		if(aux != NULL)
		{
			found = 1;
			//friend_nick = (char*)malloc(sizeof(char*));
			*friend_nick = (char*)malloc(256*sizeof(char));
			//printf("%s\n",aux);
			strcpy(*friend_nick,aux);
		}
		i++;
	}
	return found;
}

//
//
//
int copyToFile(FILE* file, char* friends[MAXFRIENDS],int num)
{
	if(num > 0)
	{
		char* aux;
		int i;

		for(i = 0; i < MAXFRIENDS; i++)
		{
			aux = friends[i];

			if(aux != NULL)
			{
				fprintf(file,"%s\n",aux);
			}
		}
	}
	return 0;
}

//
//
//
int isFileOpen(User* usr,char* friend_nick,int *pos)
{
	char* aux;
	int i = 0;
	int found = 0;
	*pos = -1;
	while( i < MAXFRIENDS && found == 0)
	{
		aux = usr->files[i]->friend_nick;
		if(aux != NULL)
		{
			if(strcasecmp(aux,friend_nick) == 0)
			{
				found = 1;
				*pos = i;
			}
		}else
		{
			if(*pos == -1)*pos = i;
		}
		i++;
	}
	return found;
}

//
//
//
int closeFiles(User* usr)
{
	if(DEBUG_MODE) printf("closeFiles -> Entrando\n");
	char* aux;
	FILE *file;
	int i;
	for(i = 0; i < MAXFRIENDS; i++)
	{
		aux = usr->files[i]->friend_nick;
		if(aux != NULL)
		{
			file = usr->files[i]->file;
			if(fclose(file) == -1) perror("Error cerrando fichero");

			free(usr->files[i]->friend_nick);
		}
	}
	if(DEBUG_MODE) printf("closeFiles -> Saliendo\n");
	return 0;
}

//
//
//
int readDownTo(User* usr,char* friend_nick,int num,char* result)
{
	/*
	size_t needed = snprintf(NULL, 0, "%s: %s (%d)", msg, strerror(errno), errno);
	    char  *buffer = malloc(needed);
	    snprintf(buffer, needed, "%s: %s (%d)", msg, strerror(errno), errno);
	*/
	char *script;
	//int size = snprintf(NULL,0,"cat %s%s/%s | tail -n %d > %s%s/.temp",DATA_PATH,usr->nick,friend_nick,num,DATA_PATH,usr->nick);
	script = (char*)malloc(256*sizeof(char));
	sprintf(script,"cat %s%s/%s | tail -n %d > %s%s/.temp",DATA_PATH,usr->nick,friend_nick,num,DATA_PATH,usr->nick);
	if(DEBUG_MODE) printf("readDownTo -> Comando: %s\n",script);
	system(script);


	char *path;
	//size = snprintf(NULL,0,"%s%s/.temp",DATA_PATH,usr->nick);
	path = (char*)malloc(256*sizeof(char));
	sprintf(path,"%s%s/.temp",DATA_PATH,usr->nick);

	if(DEBUG_MODE) printf("readDownTo -> Path: %s\n",path);

	FILE *file;
	if((file = fopen(path, "r")) == NULL) perror("Error abriendo fichero");
	if(DEBUG_MODE) printf("readDownTo -> Fichero abierto\n");
	char* aux = (char*)malloc(256*sizeof(char));

	strcpy(result,"");

	while(!feof(file))
	{
		if(fgets(aux,255,file) != NULL)
		{
			//name[strlen(name)-1] = '\0';
			//if(DEBUG_MODE) printf("readDownTo -> Fichero linea %s\n",aux);
			//sprintf(result,"%s%s\n",result,aux);
			strcat(result,aux);
			//if(DEBUG_MODE) printf("readDownTo -> Result %s\n",result);
		}
	}
	free(aux);

	if(fclose(file) == -1) perror("Error cerrando fichero");

	char *script2;
	//size = snprintf(NULL,0,"rm %s%s/.temp",DATA_PATH,usr->nick);
	script2 = (char*)malloc(256*sizeof(char));
	sprintf(script2,"rm %s%s/.temp",DATA_PATH,usr->nick);

	system(script2);

	if(DEBUG_MODE) printf("readDownTo -> Free\n");

	free(script);
	free(path);
	free(script2);

	//if(DEBUG_MODE) printf("readDownTo -> Result: %s\n",result);

	return 0;
}

#endif

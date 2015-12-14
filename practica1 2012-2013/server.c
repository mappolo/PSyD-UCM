
#include "soapH.h"
#include "ims.nsmap"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "server_file_admin.h"


#define DEBUG_MODE 0
#define DATA_PATH "server_data/"

LUser *luser;
pthread_mutex_t mutexBuffer;
struct soap soap;


void *serve_clients(struct soap *soap);
void signal_kill_handler(int sig);

int main(int argc, char **argv){ 


	/*char* nick;
	char* pass;
	printf("Nombre\n");
	nick = malloc(256*sizeof(char));
	scanf("%s",nick);

	printf("Contraseña\n");
	pass = malloc(256*sizeof(char));
	scanf("%s",pass);


	LUser *luser = (LUser*)malloc(sizeof(LUser));
	serverInit(luser);
	addUsers(luser,nick,pass);
	User *a = luser->listU[0];
	char *nombre="oscar";
	char *pass2="tetas";
	addUsers(luser,nombre,pass2);
	User *b = luser->listU[1];

	addFriend(a,b);
	removeFriend(a,b);
	removeUser(luser,"oscar");
	removeUser(luser,nick);

	serverFree(luser);*/

	signal(SIGINT, signal_kill_handler);

	int m, s;

	struct soap aux_soap;

	pthread_t idHilo;


	pthread_mutex_init (&mutexBuffer, NULL);


	/*addUsers(luser,"asd","asd");
	addUsers(luser,"asd1","asd1");
	addUsers(luser,"asd2","asd2");
	addFriendRequestSend(luser->listU[1],"asd");
	addFriendRequestSend(luser->listU[2],"asd");
	addFriendRequestPending(luser->listU[0],"asd1");
	addFriendRequestPending(luser->listU[0],"asd2");*/

  	if (argc < 2) {
    	printf("Usage: %s <port>\n",argv[0]); 
		exit(-1); 
  	}

	// Init environment
  	soap_init(&soap);

	// Bind to the specified port	
	m = soap_bind(&soap, NULL, atoi(argv[1]), 100);

	// Check result of binding		
	if (m < 0) {
  		soap_print_fault(&soap, stderr); exit(-1); 
	}


	// Inicializando las listas de usuarios etc
	luser = (LUser*)malloc(sizeof(LUser));
	serverInit(luser);
	// Listen to next connection
	while (1) { 
		// accept
	  	s = soap_accept(&soap);

	  	if (s < 0) {
			soap_print_fault(&soap, stderr); exit(-1);
	  	}

	  	aux_soap = soap;
	  	pthread_create (&idHilo, NULL, serve_clients, &aux_soap);

		// Execute invoked operation
	  	//soap_serve(&soap);

		// Clean up!
	  	//soap_end(&soap);
	}
	pthread_mutex_destroy(&mutexBuffer);
	serverFree(luser);
  return 0;
}

void *serve_clients(struct soap *soap){
	struct soap *aux_soap = soap_copy(soap);

	pthread_mutex_lock (&mutexBuffer);

  	soap_serve(aux_soap);

	pthread_mutex_unlock (&mutexBuffer);

	soap_end(aux_soap);


	//free(aux_soap);
	//aux_soap = NULL;

	return NULL;
}

void signal_kill_handler(int sig)
{

	printf("\nsignal_kill_handler -> Cerrando server...\n");

	pthread_mutex_destroy(&mutexBuffer);
	serverFree(luser);

	soap_end(&soap);

	exit(1);
}

//
//
//
int ims__addUser(struct soap *soap, char* nick, char* pass, int *error)
{
	*error = addUsers(luser,nick,pass);

	//char *path = (char*)malloc(sizeof(char*));
	char path[100];

	if(*error == 0){
		sprintf(path,"%s%s%s","mkdir ",DATA_PATH,nick);
		if(DEBUG_MODE) printf("ims__addUser -> Path: %s\n",path);
		system(path);

		sprintf(path,"%s%s%s/%s","touch ",DATA_PATH,nick,".send");
		if(DEBUG_MODE) printf("ims__addUser -> Path: %s\n",path);
		system(path);

		sprintf(path,"%s%s%s/%s","touch ",DATA_PATH,nick,".pending");
		if(DEBUG_MODE) printf("ims__addUser -> Path: %s\n",path);
		system(path);

		sprintf(path,"%s/%s/%s",DATA_PATH,nick,".pass");
		FILE *file ;
		if((file= fopen(path,"w+")) == NULL){
			perror("Error creando fichero de usuario");
		}
		if(DEBUG_MODE) printf("ims__addUser -> Path: %s\n",path);
		fprintf(file,"%s",pass);
		if(fclose(file) == -1){
			perror("Error cerrando fichero de usuario");
		}


	}

	if(DEBUG_MODE && *error == 0){
		printf("ims__addUser -> Añadido: %s %s\n",luser->listU[luser->numUser-1]->nick,luser->listU[luser->numUser-1]->pass);
	}

	return SOAP_OK;
}
int ims__removeUser(struct soap *soap, char* nick, char* pass, int *error){
	if(DEBUG_MODE) printf("ims__removeUser -> %s se quiere dar de baja\n",nick);
	removeUser(luser,nick);
	if(DEBUG_MODE) printf("ims__removeUser -> %s se ha dado de baja\n",nick);
	*error = 0;

	return SOAP_OK;
}
//
//
//
int ims__userLogin(struct soap *soap, char* nick, char* pass, int *error){
	*error = userLogin(luser,nick,pass);

	if(DEBUG_MODE && *error == 0){
		printf("ims__userLogin -> Se ha logueado: %s\n",nick);
	}
	return SOAP_OK;
}

//
//
//
int ims__userLogout(struct soap *soap, char* nick, char* pass, int *error)
{
	*error = userLogout(luser,nick,pass);
	if(DEBUG_MODE && *error == 0) printf("ims__userLogout -> %s ha cerrado sesion\n",nick);

	return SOAP_OK;
}

//
// NO SE USA
//
int ims__addFriend(struct soap *soap, char* user ,char* friend_nick, int *error){
	User *usr = getUser(luser,user);
	User *friend = getUser(luser,friend_nick);

	if(friend == NULL){
		*error = -3;
	}
	else if(usr->online == 1){
		*error = addFriend(usr,friend_nick);
		if(DEBUG_MODE && friend != NULL && *error == 0) printf("ims__addFriend -> Añadido amigo %s al usuario %s\n",friend->nick,usr->nick);
	}
	else{
		*error = -2;
	}

	return SOAP_OK;
}

//
//
//
int ims__sendFriendshipRequest(struct soap *soap, char* user ,char* friend_nick, int *error)
{
	User *usr = getUser(luser,user);
	User *friend = getUser(luser,friend_nick);
	if(usr->numFriends == MAXFRIENDS){
		*error = -5;
		return SOAP_OK;
	}

	if(friend == NULL)
	{
		*error = -3;
	}else if(usr->online == 1)
	{
		if(friend->numFriends == MAXFRIENDS){
			*error = -6;
			return SOAP_OK;
		}

		*error = isFriend(usr,friend_nick);
		if(*error == 1){
			*error = -4;
			return SOAP_OK;
		}
		*error = addFriendRequestSend(usr,friend_nick);
		if(*error == 0)
		{
			FILE *file;
			//char* path = (char*)malloc(sizeof(char*));
			char path[100];
			sprintf(path,"%s%s/.send",DATA_PATH,user);

			if(DEBUG_MODE) printf("ims__sendFriendshipRequiest -> Path: %s\n",path);

			if((file = fopen(path, "a")) == NULL) perror("Error abriendo fichero");

			fprintf(file,"%s\n",friend_nick);

			if(fclose(file) == -1) perror("Error cerrando fichero");

			//free(path);
			*error = addFriendRequestPending(friend,user);
			if(*error == 0)
			{
				//path = (char*)malloc(sizeof(char*));
				sprintf(path,"%s%s/.pending",DATA_PATH,friend_nick);

				if(DEBUG_MODE) printf("ims__sendFriendshipRequiest -> Path: %s\n",path);

				if((file = fopen(path, "a")) == NULL) perror("Error abriendo fichero");

				fprintf(file,"%s\n",user);

				if(fclose(file) == -1) perror("Error cerrando fichero");

				//free(path);

				if(DEBUG_MODE && friend != NULL && *error == 0) printf("ims__sendFriendshipRequiest -> %s envia peticion de amistad a %s\n",usr->nick,friend->nick);
			}
		}
	}else
	{
		*error = -2;
	}

	return SOAP_OK;
}

//
//
//
int ims__getFriendshipRequests(struct soap *soap, char* user,Char_vector *friends)
{
	User *usr = getUser(luser,user);

	if(usr->online == 1)
	{
		getFriendRequestsPending(usr,friends->data);

		//printf("%s\n",friends->data[0]);
		if(DEBUG_MODE)
			printf("ims__getFriendshipRequiests -> %s quiere su lista de peticiones pendientes\n",usr->nick);
	}

	return SOAP_OK;
}

//
//
//
int ims__haveFriendshipRequest(struct soap *soap, char* user,int *result)
{
	User *usr = getUser(luser,user);

	if(usr->online == 1)
	{
		*result = usr->numPending;
		if(DEBUG_MODE) printf("ims__haveFriendshipRequest -> Numero de peticiones %d\n",*result);
	}
	else{
		*result = -1;
	}

	return SOAP_OK;
}

//
//
//
int ims__getFriendshipRequest(struct soap *soap, char* user,String* friend_nick)
{
	User *usr = getUser(luser,user);
	if(usr->online == 1)
	{
		//int found =
		//char* aux = (char*)malloc(sizeof(char*));
		getFriendRequestPending(usr,&friend_nick->str);
		//friend_nick->name = aux;
		if(DEBUG_MODE) printf("ims__getFriendshipRequest -> Peticion %s\n",friend_nick->str);
		//*friend_nick = *aux;
		//printf("Primera peticion %s\n",friend_nick);
	}


	return SOAP_OK;
}

//
//
//
int ims__acceptFriendshipRequest(struct soap *soap, char* user ,char* friend_nick, int *result)
{
	User *usr = getUser(luser,user);
	User *friend = getUser(luser,friend_nick);

	if(usr->online == 1)
	{
		int found = removeFriendRequestPending(usr,friend_nick);
		if(found == 1)
		{
			found = removeFriendRequestSend(friend,user);
			if(found == 1)
			{
				addFriend(usr,friend_nick);
				addFriend(friend,user);
				//char *path = (char*)malloc(sizeof(char)*100);
				char path[100];

				sprintf(path,"touch %s%s/%s",DATA_PATH,user,friend_nick);
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Creando fichero amigos path: %s\n",path);
				system(path);

				//free(path);
				//path = (char*)malloc(sizeof(char)*100);
				//strcpy(path,"");

				sprintf(path,"touch %s%s/%s",DATA_PATH,friend_nick,user);
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Creando fichero amigos path: %s\n",path);
				system(path);



				//sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,user,".pending");
				//if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Borrando fichero pendientes path: %s\n",path);
				//system(path);
/*
				sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,friend_nick,".send");
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Borrando fichero enviados path: %s\n",path);
				system(path);

				FILE* file;

				sprintf(path,"%s%s/%s",DATA_PATH,user,".pending");
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Rescribiendo fichero pendientes path: %s\n",path);

				if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

				copyToFile(file,usr->friends_request_pending,usr->numPending);

				if(fclose(file) == -1) perror("Error cerrando fichero");

				sprintf(path,"%s%s/%s",DATA_PATH,friend_nick,".send");
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> Rescribiendo fichero enviados path: %s\n",path);

				if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

				copyToFile(file,friend->friends_request_send,friend->numSend);

				if(fclose(file) == -1) perror("Error cerrando fichero");
*/
				//free(path);

				*result = usr->numPending;
				if(DEBUG_MODE) printf("ims__acceptFriendshipRequest -> %s y %s ahora son amigos\n",usr->nick,friend->nick);
			}
		}
	}
	return SOAP_OK;
}

//
//
//
int ims__rejectFriendshipRequest(struct soap *soap, char* user ,char* friend_nick, int *result)
{
	User *usr = getUser(luser,user);
	User *friend = getUser(luser,friend_nick);

	if(usr->online == 1)
	{
		int found = removeFriendRequestPending(usr,friend_nick);
		if(found == 1)
		{
			found = removeFriendRequestSend(friend,user);
			if(found == 1)
			{
				char path[100];

				sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,user,".pending");
				if(DEBUG_MODE) printf("ims__rejectFriendshipRequest -> Borrando fichero pendientes path: %s\n",path);
				system(path);

				sprintf(path,"%s%s%s/%s","rm ",DATA_PATH,friend_nick,".send");
				if(DEBUG_MODE) printf("ims__rejectFriendshipRequest -> Borrando fichero enviados path: %s\n",path);
				system(path);

				FILE* file;

				sprintf(path,"%s%s/%s",DATA_PATH,user,".pending");
				if(DEBUG_MODE) printf("ims__rejectFriendshipRequest -> Rescribiendo fichero pendientes path: %s\n",path);

				if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

				copyToFile(file,usr->friends_request_pending,usr->numPending);

				if(fclose(file) == -1) perror("Error cerrando fichero");

				sprintf(path,"%s%s/%s",DATA_PATH,friend_nick,".send");
				if(DEBUG_MODE) printf("ims__rejectFriendshipRequest -> Rescribiendo fichero enviados path: %s\n",path);

				if((file = fopen(path, "w")) == NULL) perror("Error abriendo fichero");

				copyToFile(file,friend->friends_request_send,friend->numSend);

				if(fclose(file) == -1) perror("Error cerrando fichero");

				*result = usr->numPending;
				if(DEBUG_MODE) printf("ims__rejectFriendshipRequest -> %s y %s ahora NO son amigos\n",usr->nick,friend->nick);
			}
		}
	}

	return SOAP_OK;
}

//
//
//
int ims__removeFriend(struct soap *soap,char* user ,char* friend_nick, int *error)
{
	User* usr = getUser(luser,user);
	if(strcmp(user,friend_nick) == 0){
		*error = -3;
		return SOAP_OK;
	}
	if(usr->online == 1)
	{
		if(isFriend(usr,friend_nick) == 1)
		{
			User *friend = getUser(luser,friend_nick);
			removeFriend(usr,friend_nick);
			removeFriend(friend,user);
		}else
		{
			*error = -2;
		}
	}else
	{
		*error = -1;
	}

	return SOAP_OK;
}

//
//
//
int ims__getFriends(struct soap *soap, char* user ,Char_vector *friends)
{
	User *usr = getUser(luser,user);
	if(usr->online == 1)
	{
		getFriends(usr,friends->data);


		//printf("%s\n",friends->data[0]);
		if(DEBUG_MODE)
			printf("ims__getFriends -> %s quiere su lista de amigos\n",usr->nick);
	}

	return SOAP_OK;
}

//
//
//
int ims__haveFriends(struct soap *soap, char* user,int *result)
{
	User *usr = getUser(luser,user);

	if(usr->online == 1)
	{
		*result = usr->numFriends;
		if(DEBUG_MODE) printf("ims__haveFriends -> Numero de amigos %d\n",*result);
	}
	else{
		*result = -1;
	}

	return SOAP_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ims__getLastMessage(struct soap *soap,Message *myMessage){
	return SOAP_OK;
}

int ims__sendMessage (struct soap *soap,char* user,  Message myMessage, int *error){

	User* usr = getUser(luser,user);
	User* friend = getUser(luser,myMessage.name);
	int is_friend = 0;
	if(usr->online == 1)
	{
		is_friend = isFriend(usr,myMessage.name);
		if(is_friend == 1){

			char path[100];
			sprintf(path,"%s%s/%s",DATA_PATH,user,myMessage.name);
			if(DEBUG_MODE) printf("ims__sendMessage -> Path: %s\n",path);

			int pos = -1;
			FILE* file;
			if(isFileOpen(usr,myMessage.name,&pos) == 0)
			{
				if((file = fopen(path, "a+")) == NULL) perror("Error abriendo fichero");

				usr->files[pos]->friend_nick = (char*)malloc(256*sizeof(char));
				strcpy(usr->files[pos]->friend_nick,friend->nick);

				usr->files[pos]->file = file;
			}else
			{
				file = usr->files[pos]->file;
			}
			fprintf(file,"%s : %s\n",user,myMessage.msg);

			fflush(file);
			//if(fclose(file) == -1) perror("Error cerrando fichero");

			sprintf(path,"%s%s/%s",DATA_PATH,myMessage.name,user);
			if(DEBUG_MODE) printf("ims__sendMessage -> Path: %s\n",path);

			pos = -1;
			if(isFileOpen(friend,user,&pos) == 0)
			{
				if((file = fopen(path, "a+")) == NULL) perror("Error abriendo fichero");
				friend->files[pos]->friend_nick = (char*)malloc(256*sizeof(char));
				strcpy(friend->files[pos]->friend_nick,usr->nick);

				friend->files[pos]->file = file;
			}else
			{
				file = friend->files[pos]->file;
			}
			fprintf(file,"%s : %s\n",user,myMessage.msg);

			fflush(file);
			//if(fclose(file) == -1) perror("Error cerrando fichero");

			*error = 0;
		}
		else{
			*error = -1;
		}
	}
	else{
		*error = -2;//no esta online
	}

	return SOAP_OK;
}

/*int ims__sendMessage (struct soap *soap, struct Message myMessage, int *result){

	printf ("enviando mensaje");
	return SOAP_OK;
}*/


int ims__receiveMessage (struct soap *soap,char* user,int num,char* friend_nick,Message *myMessage)
{
	User *usr = getUser(luser,user);
	if(DEBUG_MODE) printf("ims__receiveMessage -> getUser user Friend: %s\n",friend_nick);
	User *friend = getUser(luser,friend_nick);
	if(DEBUG_MODE) printf("ims__receiveMessage -> getUser friend: \n");

	if(strcmp(user,friend_nick) == 0){
		myMessage->error = -3;
		return SOAP_OK;
	}

	if(usr->online == 1)
	{
		int is_friend = isFriend(usr,friend_nick);
		if(is_friend == 1)
		{
			//char path[100];
			//sprintf(path,"%s%s/%s",DATA_PATH,user,myMessage->name);
			//if(DEBUG_MODE) printf("ims__receiveMessage -> Path: %s\n",path);
			/*
			FILE *file;
			int pos;
			if(isFileOpen(usr,myMessage.name,&pos) == 0)
			{
				if((file = fopen(path, "a")) == NULL) perror("Error abriendo fichero");
			}else
			{
				file = usr->files[pos]->file;
			}
			//sprintf(myMessage->msg,"%s\n%s",myMessage->msg,);
			*/

			if(DEBUG_MODE) printf("ims__receiveMessage -> Entrando en downTo: \n");

			//char* result;
			myMessage->msg = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
			readDownTo(usr,friend_nick,num,myMessage->msg);

			myMessage->error = 0;
			//strcpy(myMessage->msg,result);
			//myMessage->msg = result;
			//free(result);
		}else
		{
			myMessage->error = -2;// No es amigo
		}
	}else
	{
		myMessage->error = -1;// No esta online
	}
	return SOAP_OK;
}

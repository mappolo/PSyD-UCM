#include "soapH.h"
#include "ims.nsmap"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_MODE 0
#define NUM_MESSAGES 5
#define MAXFRIENDS 100

char *user,*password;


int main(int argc, char **argv){

	struct soap soap;
	//Message myMsgA, myMsgB;
	char *serverURL;
	int res;

	//printf("Usage: %s http://server:port message\n",argv[0]);

	// Usage
  	if (argc != 2) {
  		printf("Usage: %s http://server:port message\n",argv[0]);
  		exit(0);
  	}
  	user = NULL;
  	password = NULL;
	// Init gSOAP environment
  	soap_init(&soap);

	// Obtain server address
	serverURL = argv[1];

	// Obtain message
	//msg = argv[2];
	
	// Debug?
	if (DEBUG_MODE){
		printf ("Server: %s\n", serverURL);
	}

	menuLogin(soap,serverURL);


	//Init the message read from client-side

	// Alloc memory, init to zero and copy the message
	/*myMsgA.msg = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	bzero (myMsgA.msg, IMS_MAX_MSG_SIZE);
	strcpy (myMsgA.msg, msg);

	// Alloc memory, init to zero and copy the name
	myMsgA.name = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	bzero (myMsgA.name, IMS_MAX_MSG_SIZE);
	strcpy (myMsgA.name, "Client");



	// Send this message to the server
    soap_call_ims__addUser (&soap, serverURL, "", myMsgA, &res);

 	// Check for errors...
  	if (soap.error) {
      	soap_print_fault(&soap, stderr);
		exit(1);
  	}


	// Receive a message from the server
    soap_call_ims__receiveMessage (&soap, serverURL, "", &myMsgB);

	// Check for errors...
  	if (soap.error) {
      	soap_print_fault(&soap, stderr);
		exit(1);
  	}
	else
		printf ("Server message -> user:%s - msg:%s\n", myMsgB.name, myMsgB.msg);    

  	*/
	// Clean the environment
	if(user != NULL){
		free(user);
		free(password);
	}
	soap_destroy(&soap);
  	soap_end(&soap);
  	soap_done(&soap);

  return 0;
}

int menuLogin(struct soap soap,char *serverURL){
	char *op;
	int res = -1;

	op = (char*)malloc(256*sizeof(char));
	strcpy(op,"-1");

	while(strcmp(op,"0") != 0 && res != 0){
		//system("clear");
		printf("1.-Log in.\n");
		printf("2.-Darse de alta.\n");
		printf("0.-Salir.\n");

		scanf("%s",op);

		if(strcmp(op,"1") == 0){
			res = login(soap,serverURL);
			if(res == 0){
				menuHome(soap,serverURL);
			}
			res = 1;
			strcpy(op,"-1");
		}
		else if(strcmp(op,"2") == 0){
			addNewUser(soap,serverURL);
			strcpy(op,"-1");
		}
		else if(strcmp(op,"0") == 0){
		}
		else{
			printf("Opcion no valida.\n");

		}
	}
	free(op);
	return res;
}
void menuHome(struct soap soap,char *serverURL){
	char* op;
	op = (char*)malloc(256*sizeof(char));
	int error;
	strcpy(op,"-1");
	//int choose=-1 ;
	while(strcmp(op,"0") != 0){
		//system("clear");
		printf("MENU PRINCIPAL\n");
		printf("1.-Enviar mensaje.\n");
		printf("2.-Leer mensaje.\n");
		printf("3.-Enviar peticion de amistad\n");
		printf("4.-Ver peticiones de amistad\n");
		printf("5.-Aceptar peticion de amistad\n");
		printf("6.-Ver lista de amigos\n");
		printf("7.-Eliminar amigo\n");
		printf("8.-Darse de baja\n");
		printf("0.-Salir.\n");

		scanf("%s",op);
		//choose = (int)op;
		//printf("%s\n",op);
		if(strcmp(op,"1") == 0){
			error = sendMessage(soap,serverURL);
		}
		else if(strcmp(op,"2") == 0){
			error = receiveMessage(soap,serverURL);
		}
		else if(strcmp(op,"3") == 0){
			error = sendFriendRequest(soap,serverURL);
		}
		else if(strcmp(op,"4") == 0){
			error = getFriendRequest(soap,serverURL);
		}
		else if(strcmp(op,"5") == 0){
			error = acceptFriendRequest(soap,serverURL);
		}
		else if(strcmp(op,"6") == 0){
			error = getFriends(soap,serverURL);
		}
		else if(strcmp(op,"7") == 0){
			error = removeFriend(soap,serverURL);
		}
		else if(strcmp(op,"8") == 0){
			error = removeUser(soap,serverURL);
		}
		else if(strcmp(op,"0") == 0){
			logout(soap,serverURL);
		}
		else{
			printf("Opcion no valida\n");
		}

		if(error == -1){
			strcpy(op,"0");
		}
	}
	free(op);
	free(user);
	user = NULL;
	free(password);
	password = NULL;

}

//
//
//
void addNewUser(struct soap soap,char *serverURL){

	char* nick,*pass;
	int res = 1;

	nick = (char*)malloc(256*sizeof(char));
	pass = (char*)malloc(256*sizeof(char));

	//user* usr;
	//system("clear");
	printf("\nNombre\n");
	scanf("%s",nick);
	printf("Contraseña\n");
	scanf("%s",pass);
	soap_call_ims__addUser(&soap, serverURL, "",nick,pass,&res);

	if(res == -1){
		printf("El servidor esta lleno\n");
	}
	else if(res == -2){
		printf("Ese nick esta en uso\n");
	}
	else if( res == 1){
		printf("Hay un problema con la conexion\n");
	}
	else if( res == 0){
		printf("Usuario añadido con exito\n");
	}

    free(nick);
    free(pass);
}
int removeUser(struct soap soap,char *serverURL){
	char *aux_pass = (char*)malloc(256*sizeof(char));
	int error = 1;

	printf("Introducir su contraseña para confirmar: \n");
	scanf("%s",aux_pass);
	if(strcmp(password,aux_pass) == 0){
		free(aux_pass);
		soap_call_ims__removeUser(&soap, serverURL, "",user,password,&error);
		if(error == 1){
			printf("Ha habido un problema con la conexion\n");
			return -1;
		}
		else{
			printf("Usuario eliminado\n");
			/*free(user);
			user = NULL;
			free(password);
			password = NULL;*/
			return -1;
		}
	}
	else{
		printf("Contraseña incorrecta\n");
	}
	return 0;
}

//
//
//
int login(struct soap soap,char *serverURL){
	char* nick,*pass;
	int res = 1;

	nick = (char*)malloc(256*sizeof(char));
	pass = (char*)malloc(256*sizeof(char));

	//user* usr;

	printf("\nNombre\n");
	scanf("%s",nick);
	printf("Contraseña\n");
	scanf("%s",pass);

	soap_call_ims__userLogin(&soap, serverURL, "",nick,pass,&res);

	if(res == -1){
		printf("Nombre de usuario o contraseña incorrectos\n");
		free(nick);
		free(pass);
	}
	else if (res == 1){
		printf("Hay un problema con la conexion\n");
	}
	else{
		user = nick;
		password = pass;
	}
	//free(pass);

	return res;
}

//
//
//
void logout(struct soap soap,char* serverURL)
{
	int res;
	soap_call_ims__userLogout(&soap, serverURL, "",user,password,&res);

	/*free(user);
	user = NULL;
	free(password);
	password = NULL;*/
}

//
// NO SE USA
//
void addNewFriend(struct soap soap,char *serverURL){
	char* friend_nick = (char*)malloc(256*sizeof(char));
	int result;
	system("clear");
	printf("\nNombre del amigo:\n");
	scanf("%s",friend_nick);

	soap_call_ims__addFriend(&soap, serverURL, "",user ,friend_nick, &result);

	if(result == 0){
		printf("Solicitud enviada\n");
	}
	else if(result == 1){
		printf("Ese usuario ya es tu amigo\n");
	}
	else if(result == -1){
		printf("No te puedes añadir a ti mismo\n");
	}
	else if(result == -2){
		printf("No estas logueado\n");
	}
	else if(result == -3){
		printf("Ese usuario no existe\n");
	}
	else{
		printf("Invitacion enviada correctamente:\n");
	}

	free(friend_nick);
}

//
//
//
int sendFriendRequest(struct soap soap,char *serverURL)
{
	char* friend_nick = (char*)malloc(256*sizeof(char));
	int result=1;
	system("clear");
	printf("\nNombre del amigo:\n");
	scanf("%s",friend_nick);

	soap_call_ims__sendFriendshipRequest(&soap, serverURL, "",user ,friend_nick, &result);

	if(result == 0){
		printf("Solicitud enviada\n");
	}
	else if(result == -4){
		printf("Ese usuario ya es tu amigo\n");
	}
	else if(result == -1){
		printf("No te puedes añadir a ti mismo\n");
	}
	else if(result == -2){
		printf("No estas logueado\n");
	}
	else if(result == -3){
		printf("Ese usuario no existe\n");
	}
	else if(result == 1){
		printf("Hay un problema con la conexion\n");
		return -1;
	}
	else if(result == -5){
		printf("No puedes tener mas amigos \n");
		return -1;
	}
	else if(result == -6){
		printf("Ese usuario no puede tener mas amigos\n");
		return -1;
	}

	free(friend_nick);
	return 0;

}

//
//
//
int getFriendRequest(struct soap soap,char *serverURL)
{
	Char_vector *friends = (Char_vector*)malloc(sizeof(Char_vector));

	int i;
	int numRequestPending = -2;
	soap_call_ims__haveFriendshipRequest(&soap, serverURL,"",user,&numRequestPending);
	if(numRequestPending > 0)
	{
		soap_call_ims__getFriendshipRequests(&soap, serverURL, "",user ,friends);

		system("clear");
		printf("Lista de amistades sin aceptar:\n");

		for(i=0;i < MAXFRIENDS;i++){
			if(friends->data[i] != NULL){
				printf("%d: %s\n",i,friends->data[i]);
			}
		}
	}
	else if(numRequestPending == -2){
		printf("Hay un problema con la conexion\n");
		return -1;

	}
	else if(numRequestPending == -1){
			printf("No estas online\n");
	}
	else{
		system("clear");
		printf("No tienes peticiones de amistad\n");
	}

	free(friends);

	printf("\n\n");
	return 0;

}

//
//
//
int acceptFriendRequest(struct soap soap,char* serverURL)
{
	system("clear");

	int numRequestPending = -2;
	String friend_nick;// = (xsd__string*)malloc(sizeof(xsd__string*));
	char* op = (char*)malloc(256*sizeof(char));

	soap_call_ims__haveFriendshipRequest(&soap, serverURL,"",user,&numRequestPending);
	if(numRequestPending == -2 ){
		printf("Hay un problema con la conexion\n");
		return -1;

	}
	else if(numRequestPending == -1 ){
		printf("No estas online\n");
	}
	else if(numRequestPending == 0 ){
		printf("No tienes peticiones que aceptar.\n");
	}
	else{
		while(numRequestPending > 0)
		{
			soap_call_ims__getFriendshipRequest(&soap,serverURL,"",user,&friend_nick);

			printf("%s te ha enviado una peticion de amistad\n",friend_nick.str);
			printf("Aceptar? (y/n) \n");
			scanf("%s",op);

			if(strcasecmp(op,"y") == 0 || strcasecmp(op,"yes") == 0)
			{
				soap_call_ims__acceptFriendshipRequest(&soap, serverURL,"",user,friend_nick.str,&numRequestPending);
			}else if(strcasecmp(op,"n") == 0 || strcasecmp(op,"no") == 0)
			{
				soap_call_ims__rejectFriendshipRequest(&soap, serverURL,"",user ,friend_nick.str,&numRequestPending);
			}else
			{
				printf("Opcion no valida\n");
			}
		}
	}

	printf("\n\n");

	free(op);
	return 0;

	//soap_call_ims__acceptFriendshipRequest(&soap,serverURL,"",user);
}
int removeFriend(struct soap soap,char* serverURL){
	char *friend_nick = (char*)malloc(256*sizeof(char));
	int error = 1;
	printf("Introduzca el nombre del amigo: \n");
	scanf("%s",friend_nick);

	soap_call_ims__removeFriend(&soap, serverURL, "",user ,friend_nick,&error);

	if(error == 1){
		printf("Hay un problema con la conexion \n");
		return -1;
	}
	else if (error == -1){
		printf("No estas online\n");
		return -1;
	}
	else if(error == -2){
		printf("Ese usuario no es tu amigo\n");
	}
	else if(error == -3){
		printf("No puedes borrarte a ti mismo de amigos\n");
	}
	else if(error == 0){
		printf("Amigo elimninado\n");
	}
	return 0;
}
//
//
//
int getFriends(struct soap soap,char *serverURL)
{
	Char_vector *friends = (Char_vector*)malloc(sizeof(Char_vector));

	int i;
	int numFriends = -2;
	soap_call_ims__haveFriends(&soap, serverURL,"",user,&numFriends);
	if(numFriends > 0)
	{

		soap_call_ims__getFriends(&soap, serverURL, "",user ,friends);

		system("clear");
		printf("Lista de amigos:\n");

		for(i=0;i < MAXFRIENDS;i++){
			if(friends->data[i] != NULL){
				printf("%d: %s\n",i,friends->data[i]);
			}
		}
	}else if (numFriends == 0)
	{
		//system("clear");
		printf("¡No tienes amigos!\n");
	}
	else if (numFriends == -2){
		//system("clear");
		printf("Hay un problema con la conexion\n");
		return -1;

	}
	else if (numFriends == -1){
		//system("clear");
		printf("No estas online\n");
	}

	free(friends);

	printf("\n\n");

	return 0;

}

//
//
//
int sendMessage(struct soap soap,char *serverURL)
{
	//char* friend = (char*)malloc(sizeof(char*));
	//char* message = (char*)malloc(sizeof(char*));
	Message myMessage;
	myMessage.name = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	myMessage.msg = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	int error = 1;

	system("clear");

	printf("Escriba el nombre de su amigo: ");
	scanf("%s",myMessage.name);

	//espera a leer una linea entera
	printf("Mensaje:\n");
	while(getchar()!='\n');
	fgets(myMessage.msg,IMS_MAX_MSG_SIZE,stdin);

	//limpiado de salto de linea
	 char *clean;
	 clean = strchr (myMessage.msg, '\n');
	 if (clean){
	   *clean = '\0';
	 }



	//scanf("%[^\n]s",myMessage.msg);

	soap_call_ims__sendMessage(&soap,serverURL,"",user,myMessage,&error);

	//ims__getLastMessage(&soap, serverURL, "",&myMessage);
	if(error == 0){
		printf("Mensaje enviado con exito.\n");
	}
	else if (error == 1){
		printf("Hay un problema con la conexion.\n");
		return -1;
	}
	else if (error == -1){
		printf("Ese usuario no es tu amigo, primero debes añadirle.\n");
	}
	else if (error == -2){
			printf("No estas online.\n");
	}

	free(myMessage.name);
	free(myMessage.msg);

	return 0;
}
//
//
//
int receiveMessage(struct soap soap,char *serverURL)
{
	system("clear");

	Message myMessage;
	myMessage.name = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	myMessage.msg = (xsd__string) malloc (IMS_MAX_MSG_SIZE);
	myMessage.error = 1;

	printf("Escriba el nombre de su amigo: ");
	scanf("%s",myMessage.name);

	soap_call_ims__receiveMessage(&soap,serverURL,"",user,NUM_MESSAGES,myMessage.name,&myMessage);

	if(myMessage.error == 0){
		printf("%s\n",myMessage.msg);
	}
	else if(myMessage.error == 1){
		printf("Hay un problema con la conexion\n");
		return -1;
	}
	else if(myMessage.error == -1){
		printf("No estas conectado\n");
	}
	else if(myMessage.error == -2){
		printf("Ese no es tu amigo\n");
	}
	else if(myMessage.error == -3){
		printf("No puedes leer mensajes de ti mismo, buscate un amigo\n");
	}



	free(myMessage.name);
	free(myMessage.msg);

	return 0;
}


//gsoap ns service name: ims
//gsoap ns service style: rpc
//gsoap ns service encoding: encoded
//gsoap ns service namespace: urn:ims
#define IMS_MAX_MSG_SIZE 256
#define MAXFRIENDS 100

typedef char *xsd__string;

typedef struct {
	xsd__string name;
	xsd__string msg;
	int error;
}Message;
typedef struct {
	char* data[100];
}Char_vector;

typedef struct
{
	char* str;
}String;

int ims__sendMessage ( char* user,Message myMessage, int *error);
int ims__receiveMessage (char* user,int num,char* friend_nick,Message *myMessage);
int ims__getLastMessage(Message *myMessage);

int ims__addUser(char* nick, char* pass, int *error);
int ims__removeUser(char* nick, char* pass, int *error);
int ims__userLogin(char* nick, char* pass, int *error);
int ims__userLogout(char* nick, char* pass, int *error);

int ims__addFriend(char* user ,char* friend_nick, int *error);// NO SE USA

int ims__sendFriendshipRequest(char* user ,char* friend_nick, int *error);
int ims__getFriendshipRequests(char* user ,Char_vector *friends);
int ims__haveFriendshipRequest(char* user,int *result);
int ims__getFriendshipRequest(char* user,String* friend_nick);

int ims__acceptFriendshipRequest(char* user ,char* friend_nick, int *result);
int ims__rejectFriendshipRequest(char* user ,char* friend_nick, int *result);
int ims__removeFriend(char* user ,char* friend_nick, int *error);

int ims__getFriends(char* user ,Char_vector *friends);
int ims__haveFriends(char* user,int *result);

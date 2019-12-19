#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXCLIENT 2

int main()
{  
  int iDs; 
  int maxDs; 
  int iAccept; 
  int iaClient[MAXCLIENT]; 
  
  fd_set fsStatus; 
  int iCNum = 0; // 접속자 수(client number)
  int iCounter; // 반복문을 위한 변수
  int iAddSize; 
  int iRet;
  unsigned char ucBuf[256];
  struct sockaddr_in stAddr; // 서버 주소 구조체
  struct sockaddr_in stAccept;
  
  iAddSize = sizeof(struct sockaddr_in);
  bzero(&stAddr, iAddSize); // 0으로 구조체 초기화  
 
  iDs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(iDs<0) // 예외 처리 - 소켓 생성에 실패했을 때
    {
    perror("socket() failed");
    close(iDs); // 열었던 소켓을 닫는다.  
        return -10;
    }
  stAddr.sin_family = AF_INET; // socket()의 첫번째 인자와 같다.
  // 주소 변환
  iRet = inet_pton(AF_INET, "127.0.0.1", &stAddr.sin_addr.s_addr);
  if(iRet == 0)
  {
    printf("inet_pton() failed", "invalid address string");
    close(iDs); // 열었던 소켓을 닫는다.  
    return -100;
  }
  else if(iRet < 0)
  {
    perror("inet_pton() failded");  
    close(iDs); // 열었던 소켓을 닫는다.  
    return -100;
  }
  // ip 출력  
  printf("IP : %s\n", inet_ntoa(stAddr.sin_addr));
  // 서버 포트(포트 문을 열어준다.)
  stAddr.sin_port = htons(30000); // 포트 3000번 연다.
          // 여기까지가 기본 세팅이며 client와 동일하다.
  
  if(0>bind(iDs, (struct sockaddr *)&stAddr, iAddSize))
  {
    perror("bind() failed");
    close(iDs); // 열었던 소켓을 닫는다.  
    return -100;
  }
  // 소켓이 들어오는 요청을 처리할 수 있도록 설정(listen)  
  if(0>listen(iDs, 5)) 
  {
    perror("listen() failed");
    close(iDs);
    return -100;
  }

  maxDs = iDs+1; // +1 하지 않으면 select가 한 비트 앞까지만 검사한다.
  while(1)
  {
    printf("현재 접속자 수 %d\n", iCNum); // 접속자 수 표시
    
    
    FD_ZERO(&fsStatus); // select()가 호출된후 매번 '0'으로 초기화
   
    FD_SET(iDs, &fsStatus);  // 디스크립터, 소켓식별자 벡터
    FD_SET(1, &fsStatus); // select가 0번(서버의 키보드 입력)까지 감시한다.
    // 접속 인원 체크
    for(iCounter=0;iCNum>iCounter;++iCounter)
    {
      FD_SET(iaClient[iCounter], &fsStatus);
    }
   
    if(0 > select(maxDs, &fsStatus, NULL, NULL, NULL))
    {
      perror("select() error... ");
      close(iDs);
      return -100;
    }
    printf("Select End\n");
    
    if(1 == FD_ISSET(0, &fsStatus)) // 서버의 키보드 입력 확인
    {
      iRet = read(0, ucBuf, sizeof(ucBuf));
      ucBuf[iRet] = 0;
      printf("%s\n", ucBuf);
      
      for(iCounter=0;iCNum>iCounter;++iCounter)// 전체 접속 인원 검색
      {
        write(iaClient[iCounter], ucBuf, iRet);
      }    
      continue;
    }
  
    // 접속되지 않았던 유저가 접속하려하면 if문 안으로 들어간다.(accept 수행)
    if(1 == FD_ISSET(iDs, &fsStatus)) // return 값이 1이면 참
    {
      iAccept = accept(iDs, (struct sockaddr *)&stAccept, &iAddSize);
      if(0>iAccept)  
      {
        perror("accept() error");
        close(iDs); // 랑데뷰 함수를 닫는다.
        // 들어온 접속자 만큼 닫는다.- 접속자가 없으면 돌지 않는다.
        for(iCounter=0;iCNum>iCounter;++iCounter)
        {
          close(iaClient[iCounter]);
        }
        return -100;
      }
      // 접속 제한 인원보다 많이 들어 왔는가 확인
      if(MAXCLIENT <= iCNum)
      {
        write(iAccept, "Server is full connection\n", sizeof("Server is full connection\n"));
        close(iAccept);
        continue ;
      }
      // 중간에 빈 파일디스크립터로 생성되었을 경우에는 더하면 안되므로
      if(iAccept == maxDs)
      {
        maxDs = iAccept + 1;   
        iaClient[iCNum] = iAccept;
        ++iCNum;
      }    
      // 접속이 오면 상대방 IP가 출력된다.  
      // 파일로 생각하면 완전히 파일이 열린 상태
      printf("Client IP : [%s]\n", inet_ntoa(stAccept.sin_addr));
      // 다시 처음으로 돌아간다.
      continue ;
    }
    // 커뮤니케이션 소켓이면 위의 if문을 빠져나온다.
    printf("Communimation socket input\n");
    // 들어온 쪽을 찾기 위해 열려진 부분을 모두 찾아야 한다.
    for(iCounter=0;iCNum>iCounter;++iCounter)// 전체 접속 인원 검색
    {
      // 해당하는 Client에 1이 세팅(입력이 들어왔는가)
      // 사용자 소켓 번호 체크
      if(1==FD_ISSET(iaClient[iCounter], &fsStatus))
      {
        // 저수준으로 읽는다. ucBuf의 길이를 알기 위해 iRet로 반환값을 저장한다.
        iRet = read(iaClient[iCounter], ucBuf, sizeof(ucBuf));
        ucBuf[iRet] = 0;
        printf("%s\n", ucBuf);
        for(iCounter=0;iCNum>iCounter;++iCounter)// 전체 접속 인원 검색
        {
          write(iaClient[iCounter], ucBuf, iRet);
        }    
      }
    }    
  }
  write(iAccept, "pong", sizeof("pong"));
  close(iDs); // 열었던 소켓을 닫는다.  
  close(iAccept);
  return 0;
}

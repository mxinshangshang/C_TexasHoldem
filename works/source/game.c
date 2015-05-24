#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>

int m_socket_id = -1;

FILE *DataFServer;
time_t now;
struct tm *curTime;
char filename[256];

char hold[] = "hold/(.*)/hold";
char flop[] = "^(flop/\n)(.*)(/flop\n)$";
char turn[] = "^(turn/\n)(.*)(/turn\n)$";
char river[] = "^(river/\n)(.*)(/river\n)$";
regex_t reg;
size_t  len;
int err;
char errbuf[1024];
int nm=10;
regmatch_t pmatch[10];

char* CARDS[13]={"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
int REPEAT[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
char* Cards[7];
char* color[7];
char* num[7];
int re_num=0;
int n=0;
int call=0;
int check=0;
int fold=0;

/* 处理server的消息 */
int on_server_message(int length, const char* buffer)
{
    char reg_msg[50] = {'\0'};
    char *buf;
    char *result = NULL;
    char dest[100] = {0};
    char *p, *p1, *p2;
    int i,j;
    int dui=0;
    DataFServer=fopen(filename,"a+");
    printf("Recieve Data From Server___________\n%s\n", buffer);
    //fprintf(DataFServer,"Recieve Data From Server___________\n%s\n", buffer);
    
    if(regcomp(&reg,hold,REG_EXTENDED)<0)
    {
        regerror(err,&reg,errbuf,sizeof(errbuf));
        printf("error: regcomp:%s\n",errbuf);
    }
    err = regexec(&reg,buffer,(size_t)nm,pmatch,0);
    if(err == REG_NOMATCH)
    {
        printf("no match\n");
        //exit(-1);
    }
    else if(err)
    {
        regerror(err,&reg,errbuf,sizeof(errbuf));
        printf("err:%s\n",errbuf);
        //exit(-1);
    }    

    //for(i=0;i<10 && pmatch[i].rm_so!=-1;i++)
    //{
    //   int len;
       len = pmatch[0].rm_eo-pmatch[0].rm_so;
       if(len!=0)
       {
          memset(dest,'\0',sizeof(dest));
          memcpy(dest,buffer+pmatch[0].rm_so,len);
          //printf("%s\n",match);
          //printf("Recieve Data From Server___________\n%s\n", dest);
          fprintf(DataFServer,"__________hold:\n%s\n", dest);
       }
    //}
    regfree(&reg);

    

    if(strstr(buffer,"inquire")!=NULL)
    {
        if(1==call)
        {
           call=0;
           snprintf(reg_msg, sizeof(reg_msg) - 1, "call\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);
        }
        else if(1==fold)
        {
           fold=0;
           snprintf(reg_msg, sizeof(reg_msg) - 1, "fold\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);       
        }
        else
        { 
           check=0; 
           snprintf(reg_msg, sizeof(reg_msg) - 1, "check\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);               
        }
    }
    if(strstr(buffer,"pot-win")!=NULL)
    {
        for(i=0;i<13;i++)
        {
            REPEAT[i]=0;
        }
    }
    fclose(DataFServer);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
       printf("Usage: ./%s server_ip server_port my_ip my_port my_id\n", argv[0]);
       return -1;
    }

    /* 获取输入参数 */
    in_addr_t server_ip = inet_addr(argv[1]);
    in_port_t server_port = htons(atoi(argv[2])); 
    in_addr_t my_ip = inet_addr(argv[3]);
    in_port_t my_port = htons(atoi(argv[4])); 
    int my_id = atoi(argv[5]);

    /* 创建socket */
    m_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket_id < 0)
    {
       printf("init socket failed!\n");
       return -1;
    }

    /* 设置socket选项，地址重复使用，防止程序非正常退出，下次启动时bind失败 */
    int is_reuse_addr = 1;
    setsockopt(m_socket_id, SOL_SOCKET, SO_REUSEADDR, (const char*)&is_reuse_addr, sizeof(is_reuse_addr));

    /* 绑定指定ip和port，不然会被server拒绝 */
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = my_ip;
    my_addr.sin_port = my_port;
    if(bind(m_socket_id, (struct sockaddr*)&my_addr, sizeof(my_addr)))
    {
       printf("bind failed!\n"); 
       return -1;
    }

    /* 连接server */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip;
    server_addr.sin_port = server_port;
    while(connect(m_socket_id, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
   {
      usleep(100*1000); /* sleep 100ms, 然后重试，保证无论server先起还是后起，都不会有问题 */
   }

    /* 向server注册 */
    char reg_msg[50] = {'\0'};
    snprintf(reg_msg, sizeof(reg_msg) - 1, "reg: %d %s \n", my_id, "Mxin"); 
    send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);

    /* 建立随机文件保存牌信息 */
    now = time(NULL);
    curTime = localtime(&now);
    sprintf(filename,"%04d-%02d-%02d %02d-%02d-%02d.txt",curTime->tm_year+1900,
        curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
        curTime->tm_sec);

    /* 接收server消息，进入游戏 */
   while(1)
   {
      char buffer[1024] = {'\0'};
      int length = recv(m_socket_id, buffer, sizeof(buffer) - 1, 0);
      if(length > 0)
      { 
          if (-1 == on_server_message(length, buffer))
          {
             break;
          }
      } 
   }
   /* 关闭socket */
   close(m_socket_id);
   return 0;
}

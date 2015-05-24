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
    char buf[128] = {0};
    char buf1[128] = {0};
    char buf2[128] = {0};
    char buf3[128] = {0};
    char buf4[128] = {0};
    char buf5[128] = {0};
    char buf6[128] = {0};
    char buf7[128] = {0};
    char buf8[128] = {0};
    char buf9[128] = {0};
    char buf10[128] = {0};
    char buf11[128] = {0};
    char buf12[128] = {0};
    char buf13[128] = {0};
    char buf14[128] = {0};
    char dest[100] = {0};
    char *p, *p1, *p2;
    int i,j;
    int dui=0;
    DataFServer=fopen(filename,"a+");
    //fprintf(DataFServer,"____________________________________________Server\n%s\n", buffer);
    if(strstr(buffer,"seat")!=NULL)         //  hold 
    {
       fprintf(DataFServer,"____________________________________________Server\n");
    }    


    if(strstr(buffer,"hold")!=NULL)         //  hold 
    {
        p1 = strstr(buffer, "hold/");
        p2 = strstr(buffer, "/hold");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("hold/\n");
            memcpy(dest, p1, p2 - p1);
            sscanf(dest,"%[^ ]%*s %*s %*s",buf1);
            color[0]=buf1;
            sscanf(dest,"%*s %[^ ]%*s %*s",buf2);
            num[0]=buf2;
            sscanf(dest,"%*s %*s %[^ ] %*s",buf3);
            color[1]=buf3;
            sscanf(dest,"%*s %*s %*s %[^ ]",buf4);
            num[1]=buf4;
            //printf("hold:%s %s %s %s\n", color[0], num[0], color[1], num[1]);
            fprintf(DataFServer,"hold:%s %s %s %s\n", color[0], num[0], color[1], num[1]);
            check=1;
        }    
    }
    if(strstr(buffer,"flop")!=NULL)               //  flop 
    {
        p1 = strstr(buffer, "flop/");
        p2 = strstr(buffer, "/flop");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("flop/\n");
            memcpy(dest, p1, p2 - p1);
            sscanf(dest,"%[^ ]%*s %*s %*s %*s %*s",buf5);
            color[2]=buf5;
            sscanf(dest,"%*s %[^ ]%*s %*s %*s %*s",buf6);
            num[2]=buf6;
            sscanf(dest,"%*s %*s %[^ ] %*s %*s %*s",buf7);
            color[3]=buf7;
            sscanf(dest,"%*s %*s %*s %[^ ] %*s %*s",buf8);
            num[3]=buf8;
            sscanf(dest,"%*s %*s %*s %*s %[^ ] %*s",buf9);
            color[4]=buf9;
            sscanf(dest,"%*s %*s %*s %*s %*s %[^ ]",buf10);
            num[4]=buf10;
            //printf("flop:%s %s %s %s %s %s\n", color[2], num[2], color[3], num[3], color[4], num[4]); 
            fprintf(DataFServer,"flop:%s %s %s %s %s %s\n", color[2], num[2], color[3], num[3], color[4], num[4]);
            for(i=0;i<13;i++)
            {
                for(j=i+1;j<5;j++)
                {
                   if(CARDS[i]==num[j])
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<13;i++)
            {
                if(REPEAT[i]>=2)
                {
                   re_num++;
                   REPEAT[i]=0;                  
                }
            }
            printf("----------------5re_num:%d\n", re_num);
            if(re_num>=2)
            {
                call=0; 
                check=1;
                fold=0;
            }
            else
            {
                call=0; 
                check=0;
                fold=1;
            }
            re_num=0;
        }    
    }
    if(strstr(buffer,"turn")!=NULL)          //  turn 
    {
        p1 = strstr(buffer, "turn/");
        p2 = strstr(buffer, "/turn");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("turn/\n");
            memcpy(dest, p1, p2 - p1);
            sscanf(dest,"%[^ ]%*s",buf11);
            color[5]=buf11;
            sscanf(dest,"%*s %[^ ]",buf12);
            num[5]=buf12;
            //printf("turn:%s %s\n", color[5], num[5]);
            fprintf(DataFServer,"turn:%s %s\n", color[5], num[5]);
            for(i=0;i<13;i++)
            {
                for(j=i+1;j<6;j++)
                {
                   if(CARDS[i]==num[j])
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<13;i++)
            {
                if(REPEAT[i]>=2)
                {
                   re_num++;
                   //fprintf(DataFServer,"%d\n", REPEAT[i]);
                   REPEAT[i]=0;                  
                }
            }
            printf("----------------6re_num:%d\n", re_num);
            if(re_num>=2)
            {
                call=0; 
                check=1;
                fold=0;
            }
            else
            {
                call=0; 
                check=0;
                fold=1;
            }
            re_num=0;
        }    
    }
    if(strstr(buffer,"river")!=NULL)        //  river
    {
        p1 = strstr(buffer, "river/");
        p2 = strstr(buffer, "/river");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("river/\n");
            memcpy(dest, p1, p2 - p1);
            sscanf(dest,"%[^ ]%*s",buf13);
            color[6]=buf13;
            sscanf(dest,"%*s %[^ ]",buf14);
            num[6]=buf14;
            //printf("river:%s %s\n", color[6], num[6]);
            fprintf(DataFServer,"river:%s %s\n", color[6], num[6]);
            for(i=0;i<13;i++)
            {
                for(j=i+1;j<7;j++)
                {
                   if(CARDS[i]==num[j])
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<13;i++)
            {
                if(REPEAT[i]>=2)
                {
                   re_num++;
                   //fprintf(DataFServer,"%d\n", REPEAT[i]);
                   REPEAT[i]=0;                  
                }
            }
            printf("----------------7re_num:%d\n", re_num);
            if(re_num>=2)
            {
                call=0; 
                check=1;
                fold=0;
            }
            else
            {
                call=0; 
                check=0;
                fold=1;
            }
            re_num=0;
            n=0;
        }    
    }
    if(strstr(buffer,"inquire")!=NULL)
    {
        if(call==1)
        {
           call=0;
           snprintf(reg_msg, sizeof(reg_msg) - 1, "call\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);
        }
        else if(fold==1)
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

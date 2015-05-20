#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int m_socket_id = -1;

FILE *DataFServer;
char randstr[25];
char* Cards[7];
char* color[7];
char* num[7];
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
    printf("Recieve Data From Server(%s)\n", buffer);
    if(strstr(buffer,"seat")!=NULL)
    {
        fprintf(DataFServer,"%s", "\n\n");
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
            result = strtok(dest,"\n");
            while( result != NULL ) 
            {
               Cards[n++]=result;
               //fprintf(DataFServer,"%s", Cards[n-1]);
               result = strtok( NULL,"\n");
            }
            color[0]=strtok(Cards[0]," ");        //第一张手牌
            fprintf(DataFServer,"%s ", color[0]); 
            num[0]=strtok(NULL," ");
            fprintf(DataFServer,"%s ", num[0]); 
            color[1]=strtok(Cards[1]," ");        //第二张手牌
            fprintf(DataFServer,"%s ", color[1]); 
            num[1]=strtok(NULL," ");
            fprintf(DataFServer,"%s \n", num[1]); 

            call=1;
        }    
    }
    if(strstr(buffer,"flop")!=NULL)          //  flop 
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
            result = strtok(dest,"\n");
            while( result != NULL ) 
            {
               Cards[n++]=result;
               //fprintf(DataFServer,"%s", Cards[n-1]);
               result = strtok( NULL,"\n");
            } 
            color[2]=strtok(Cards[2]," ");            //第三张手牌
            fprintf(DataFServer,"%s ", color[2]); 
            num[2]=strtok(NULL," ");
            fprintf(DataFServer,"%s ", num[2]); 
            color[3]=strtok(Cards[3]," ");            //第四张手牌
            fprintf(DataFServer,"%s ", color[3]); 
            num[3]=strtok(NULL," ");
            fprintf(DataFServer,"%s ", num[3]); 
            color[4]=strtok(Cards[4]," ");            //第五张手牌
            fprintf(DataFServer,"%s ", color[4]); 
            num[4]=strtok(NULL," ");
            fprintf(DataFServer,"%s \n", num[4]);
            /*for(i=0;i<7;i++)
            {
                for(j=i+1;j<7;j++)
                {
                   if(num[i]==num[j])
                   {
                      dui++;
                   }
                }
            }
            if(dui!=0)
            {
               call=1;
            }
            else if(dui>=3)
            {
               check=1;           
            }
            else
            {
               fold=1;           
            }*/
            fprintf(DataFServer,"%d \n", dui-1);
            dui=0;

            check=1;
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
            result = strtok(dest,"\n");
            while( result != NULL ) 
            {
               Cards[n++]=result;
               //fprintf(DataFServer,"%s", Cards[n-1]);
               result = strtok( NULL,"\n");
            }
            check=1;  
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
            result = strtok(dest,"\n");
            while( result != NULL ) 
            {
               Cards[n++]=result;
               //fprintf(DataFServer,"%s", Cards[n-1]);
               result = strtok( NULL,"\n");
            }
            check=1;   
            n=0;
        }    
    }
    if(strstr(buffer,"inquire")!=NULL)
    {
        if(call==1)
        {
           snprintf(reg_msg, sizeof(reg_msg) - 1, "call\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);
           call=0;
        }
        else if(check==1)
        {
           snprintf(reg_msg, sizeof(reg_msg) - 1, "check\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0); 
           check=0;           
        }
        else if(fold==1)
        {
           snprintf(reg_msg, sizeof(reg_msg) - 1, "fold\n"); 
           send(m_socket_id, reg_msg, strlen(reg_msg) + 1, 0);    
           fold=0;          
        }
    }
    if(strstr(buffer,"pot-win")!=NULL)
    {

    }
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
    int randnum = rand();
    sprintf(randstr, "%d", randnum);
    strcat(randstr,".txt");
    DataFServer=fopen(randstr,"a+");

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
   fclose(DataFServer);
   return 0;
}

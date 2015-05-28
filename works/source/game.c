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
FILE *Server;
time_t now;
struct tm *curTime;
char filename[256];

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
char* CARDS[13]={"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
int REPEAT[13]={0,0,0,0,0,0,0,0,0,0,0,0,0};
char* COLOR[4]={"SPADES","CLUBS","DIAMONDS","HEARTS"};
int REPEAT_C[4]={0,0,0,0};
char* Cards[7];
char* color[7];
char* num[7];
int pair_num=0;
int three_num=0;
int four_num=0;
int n=0;
int is_hold=0;
int is_first_time=0;
int player_num=0;
int call=0;
int all_in=0;
int check=0;
int fold=0;
int must_be=0;

int findsub(char* src, char* s)
{
    char *ptr=src, *p=s;
    /* ptr2为src的末位置指针 */                       
    char *ptr2=src+strlen(src), *prev=NULL;
    /* 子串的长度和计数器 */    
    int len=strlen(s), n=0;                    
    for(;*ptr;ptr++)
    {
    /* 如果一开始子串就大于src,则退出 */
        if(ptr2-ptr<len) 
            break;
    /* 寻找第一个相等的位置,然后从此位置开始匹配子串 */
        for(prev=ptr;*prev==*p;prev++,p++)
        {
    /* 如果已经到了子串的末尾 */
            if(*(p+1)==0||*(p+1)==10)          
            {
                n++;
    /* 重新指向子串 */
                p=s; 
                break;
            }
        }
    }
    return n;
}

/* 处理server的消息 */
int on_server_message(int length, const char* buffer)
{
    char reg_msg[50] = {'\0'};
    char dest[256] = {0};
    char *p, *p1, *p2;
    int i,j,a,b,c;
    int dui=0;
    DataFServer=fopen(filename,"a+");
    Server=fopen("server.txt","a+");    
    fprintf(Server,"____________________________________________Server\n%s\n", buffer);
    //fprintf(DataFServer,"____________________________________________\n");
    if(strstr(buffer,"/seat")!=NULL)         //  seat 
    {            
        check=1;
        call=0; 
        fold=0; 
        p1 = strstr(buffer, "seat/");
        p2 = strstr(buffer, "/seat");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("seat/\n");
            memcpy(dest, p1, p2 - p1);
            //if(is_first_time==0)
            //{
                //is_first_time=1;
                player_num=findsub(dest,"\n")-2;
                fprintf(Server,"player_num:%d\n", player_num);
            //}
        }
    }    

    if(strstr(buffer,"/hold")!=NULL)         //  hold 
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
            for(i=0;i<4;i++)
            {
                for(j=0;j<2;j++)
                {
                   if(strstr(color[j],COLOR[i])!=NULL)
                   {
                      REPEAT_C[i]++;
                   }
                }
            }
            for(i=0;i<13;i++)
            {
                for(j=0;j<2;j++)
                {
                   if(strstr(num[j],CARDS[i])!=NULL)
                   {
                      REPEAT[i]++;
                   }
                }
            }

            for(i=12;i>=0;i--)
            {
                if(REPEAT[i]==2)
                {
                   if(i<8)//>=10 pair
                   {
                      check=0;
                      call=0; 
                      fold=1; 
                      break;
                   }
                   else if(i>10)
                   {
                      must_be=1;
                      check=1;
                      call=0; 
                      fold=0; 
                      break;
                   }
                   else
                   {
                      check=1;
                      call=0; 
                      fold=0; 
                      break;
                   }
                }
                else if(REPEAT[i]==1)
                {
                   if(REPEAT[i-1]==1)
                   {
                      if(i>=10)//J Q up
                      {
                        call=0; 
                        check=1;
                        fold=0; 
                        break; 
                      }
                      else if(i>7 && i<10)//9 10 up
                      {
                        for(j=0;j<4;j++)
                        {
                           if(REPEAT_C[j]==2)
                           {
                             call=0; 
                             check=1;
                             fold=0; 
                             break;        
                            }
                         }
                      }
                      else
                      {
                         check=0;
                         call=0; 
                         fold=1; 
                         break;                         
                      }
                    }
                    else
                    {
                        check=0;
                        call=0; 
                        fold=1; 
                        break;                         
                    }                    
                 }
            }
            for(i=0;i<13;i++)
            {
                REPEAT[i]=0;
            }
            for(i=0;i<4;i++)
            {
                REPEAT_C[i]=0;
            }
        }
        is_hold=1;    
    }
    if(strstr(buffer,"/flop")!=NULL)               //  flop 
    {    
        is_hold=0; 
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

            for(i=0;i<4;i++)
            {
                for(j=0;j<5;j++)
                {
                   if(strstr(color[j],COLOR[i])!=NULL)
                   {
                      REPEAT_C[i]++;
                   }
                }
            }
            for(i=0;i<4;i++)
            {
                if(REPEAT_C[i]>=4)
                {
                   call=0; 
                   check=1;
                   fold=0;
                   break;                
                }
            }
            for(i=0;i<4;i++)
            {
                REPEAT_C[i]=0;
            }

            for(i=0;i<13;i++)
            {
                for(j=0;j<5;j++)
                {
                   if(strstr(num[j],CARDS[i])!=NULL)
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<9;i++)
            {
                if(REPEAT[i]!=0)
                {   
                    if(REPEAT[i+1]!=0 && REPEAT[i+2]!=0 && REPEAT[i+3]!=0 && REPEAT[i+4]!=0)
                    {
                        call=0; 
                        check=1;
                        fold=0;
                        break;
                    }
                }
            }
                              
            for(i=0;i<13;i++)
            {
                if(REPEAT[i]==2)
                {
                   pair_num++;                 
                }
                if(REPEAT[i]==3)
                {
                   three_num++;                
                }
                if(REPEAT[i]>3)
                {
                   four_num++;               
                }
            }
            for(i=0;i<13;i++)
            {
                REPEAT[i]=0;
            }
            //printf("- - - - - - - - - - - - - 5- - re_num:%d\n", re_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 5- - pair_num:%d\n", pair_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 5- - three_num:%d\n", three_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 5- - four_num:%d\n", four_num);
            if(pair_num>=1 || three_num>=1)
            {
               call=0; 
               check=1;
               fold=0;               
            }
            if(four_num==1)
            {
               all_in=1;
               call=0; 
               check=0;
               fold=0;                 
            }
            pair_num=0;
            three_num=0;
            four_num=0;
            if(check!=1 && all_in!=1)
            {
                fold=1;
            }
        }    
    }
    if(strstr(buffer,"/turn")!=NULL)          //  turn 
    {
        is_hold=0; 
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
            for(i=0;i<4;i++)
            {
                for(j=0;j<6;j++)
                {
                   if(strstr(color[j],COLOR[i])!=NULL)
                   {
                      REPEAT_C[i]++;
                   }
                }
            }
            for(i=0;i<4;i++)
            {
                if(REPEAT_C[i]>=5)
                {
                   call=0; 
                   check=1;
                   fold=0;   
                   break;             
                }
            }
            for(i=0;i<4;i++)
            {
                REPEAT_C[i]=0;
            }

            for(i=0;i<13;i++)
            {
                for(j=0;j<6;j++)
                {
                   if(strstr(num[j],CARDS[i])!=NULL)
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<9;i++)
            {
                if(REPEAT[i]!=0)
                {   
                    if(REPEAT[i+1]!=0 && REPEAT[i+2]!=0 && REPEAT[i+3]!=0 && REPEAT[i+4]!=0)
                    {
                        call=0; 
                        check=1;
                        fold=0;
                        break;
                    }
                }
            }

            for(i=0;i<13;i++)
            {
                if(REPEAT[i]==2)
                {
                   pair_num++;                 
                }
                if(REPEAT[i]==3)
                {
                   three_num++;                
                }
                if(REPEAT[i]>3)
                {
                   four_num++;               
                }
            }
            for(i=0;i<13;i++)
            {
                REPEAT[i]=0;
            }
            //printf("- - - - - - - - - - - - - - 6- re_num:%d\n", re_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - 6- pair_num:%d\n", pair_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - 6- three_num:%d\n", three_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - 6- four_num:%d\n", four_num);
            if(pair_num>=1 || three_num>=1)
            {
               call=0; 
               check=1;
               fold=0;               
            }
            if(four_num==1)
            {
               all_in=1;
               call=0; 
               check=0;
               fold=0;               
            }
            pair_num=0;
            three_num=0;
            four_num=0;
            if(check!=1 && all_in!=1)
            {
                fold=1;
            }
        }    
    }
    if(strstr(buffer,"/river")!=NULL)        //  river
    {
        is_hold=0; 
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
            for(i=0;i<4;i++)
            {
                for(j=0;j<7;j++)
                {
                   if(strstr(color[j],COLOR[i])!=NULL)
                   {
                      REPEAT_C[i]++;
                   }
                }
            }
            for(i=0;i<4;i++)
            {
                if(REPEAT_C[i]>=5)
                {
                   call=0; 
                   check=1;
                   fold=0;  
                   break;              
                }
            }
            for(i=0;i<4;i++)
            {
                REPEAT_C[i]=0;
            }

            for(i=0;i<13;i++)
            {
                for(j=0;j<7;j++)
                {
                   if(strstr(num[j],CARDS[i])!=NULL)
                   {
                      REPEAT[i]++;
                   }
                }
            }
            for(i=0;i<9;i++)
            {
                if(REPEAT[i]!=0)
                {   
                    if(REPEAT[i+1]!=0 && REPEAT[i+2]!=0 && REPEAT[i+3]!=0 && REPEAT[i+4]!=0)
                    {
                        call=0; 
                        check=1;
                        fold=0;
                        break;
                    }
                }
            }

            for(i=0;i<13;i++)
            {
                if(REPEAT[i]==2)
                {
                   pair_num++;                 
                }
                if(REPEAT[i]==3)
                {
                   three_num++;                
                }
                if(REPEAT[i]>3)
                {
                   four_num++;               
                }
            }
            for(i=0;i<13;i++)
            {
                REPEAT[i]=0;
            }
            //printf("- - - - - - - - - - - - - - - 7re_num:%d\n", re_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 7pair_num:%d\n", pair_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 7three_num:%d\n", three_num);
            fprintf(DataFServer,"- - - - - - - - - - - - - - - 7four_num:%d\n", four_num);
            if(pair_num>=1 || three_num>=1)
            {
               call=0; 
               check=1;
               fold=0;               
            }
            if(four_num==1)
            {
               all_in=1;
               call=0; 
               check=0;
               fold=0;                  
            }
            pair_num=0;
            three_num=0;
            four_num=0;
            if(check!=1 && all_in!=1)
            {
                fold=1;
            }
            n=0;
        }    
    }
    if(strstr(buffer,"/inquire")!=NULL)
    {
        p1 = strstr(buffer, "inquire/");
        p2 = strstr(buffer, "/inquire");
        if (p1 == NULL || p2 == NULL || p1 > p2) 
        {
            printf("Not found\n");
        }
        else 
        {
            p1 += strlen("inquire/\n");
            memcpy(dest, p1, p2 - p1);
            if(strstr(dest, "fold")!=NULL)
            {
                i=findsub(dest,"fold");
                if(i==player_num)
                {
                   check=1;
                   fold=0; 
                }
            }
            if(strstr(dest, "all_in")!=NULL && is_hold==1 && must_be==0)
            {
               is_hold=0;
               check=0;
               fold=1;
            }
            must_be=0;
        }

        if(1==all_in)
        {
           all_in=0;
           snprintf(reg_msg, sizeof(reg_msg) - 1, "all_in\n"); 
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
        for(i=0;i<4;i++)
        {
            REPEAT_C[i]=0;
        }
    }
    fclose(DataFServer);
    fclose(Server);
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
      char buffer[256] = {'\0'};
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

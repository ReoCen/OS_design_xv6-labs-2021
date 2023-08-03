#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char args[MAXARG][512];    //用于存储命令和参数
char *pass_args[MAXARG];   //用于传递给exec函数的参数
int preargnum, argnum;     //表示预先给定的参数数量,当前行的参数数量
char ch;     //用于读取字符的临时变量
char arg_buf[512];   //用于临时存储每个参数
int n;

//用于从标准输入中读取一行内容，并将其解析为参数
int readline()
{
    argnum = preargnum;
    // memset(args, 0, sizeof(args));
    memset(arg_buf, 0, sizeof(arg_buf));
    for (;;)
    {
        n = read(0, &ch, sizeof(ch));
        if (n == 0)     //已经读取完所有内容
        {
            return 0;
            break;
        }
        else if (n < 0)
        {
            fprintf(2, "read error\n");
            exit(1);
        }
        else
        {
            if (ch == '\n')   //取到的字符是换行符,表示当前行的参数已经读取完
            {
                memcpy(args[argnum], arg_buf, strlen(arg_buf) + 1);
                argnum++;
                return 1;
            }
            else if (ch == ' ')
            {
                memcpy(args[argnum], arg_buf, strlen(arg_buf) + 1);
                argnum++;
                memset(arg_buf, 0, sizeof(arg_buf));
            }
            else
            {
                arg_buf[strlen(arg_buf)] = ch;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)   //命令行参数的数量必须大于2
    {
        printf("usage: xargs [command] [arg1] [arg2] ... [argn]\n");
        exit(0);
    }
    preargnum = argc - 1;
    for (int i = 0; i < preargnum; i++)   //将命令行参数复制到args数组中。
        memcpy(args[i], argv[i + 1], strlen(argv[i + 1]));
    while (readline())
    {
        if (fork() == 0)   //创建子进程
        {
            *args[argnum] = 0;   //表示参数列表的结尾
            int i = 0;
            while (*args[i])
            {
                //每个参数的地址赋值给pass_args数组。
                pass_args[i] = (char *)&args[i];
                i++;
            }
            *pass_args[argnum] = 0;   //表示传递给exec函数的参数列表的结尾
            exec(pass_args[0], pass_args);   //在子进程中执行指定的命令和参数
            printf("exec error\n");
            exit(0);
        }
        else
        {
            wait((int *)0);   //调用wait函数等待子进程的结束
        }
    }
    exit(0);
}
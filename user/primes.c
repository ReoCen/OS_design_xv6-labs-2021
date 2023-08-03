#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int MAX_NUM = 35;

int p1[2], fdr, fdw;   //存储管道的读写文件描述符，读取和写入管道的文件描述符
long p, n;     //当前质数和待筛选数值
int is_first = 1;

int main(int argc, char *argv[])
{
    if (is_first == 1)   //是否是第一个进程
    {
        is_first = 0;
        //创建管道
        pipe(p1);
        fdr = p1[0];
        fdw = p1[1];
        for (n = 2; n <= MAX_NUM; n++)   //循环将数字2到MAX_NUM写入管道
        {
            write(fdw, (void *)&n, 8);
        }
        close(fdw);
    }
    if (fork() == 0)   //创建子进程
    {
        if (read(fdr, (void *)&p, 8))    //成功读取到质数
        {
            printf("prime %d\n", p);
            pipe(p1);
            fdw = p1[1];
        }

        while (read(fdr, (void *)&n, 8))    //从管道中读取数值n
        {
            if (n % p != 0)    //如果n不能被当前质数p整除，则将其写入新的管道
                write(fdw, (void *)&n, 8);
        }
        fdr = p1[0];
        close(fdw);
        main(argc, argv);   //递归调用main函数
    }
    else
    {
        wait((int *)0);
        close(fdr);
    }

    exit(0);
}
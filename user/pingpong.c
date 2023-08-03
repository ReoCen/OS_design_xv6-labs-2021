#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p1[2], p2[2], pid; //存储管道的读写文件描述符
    char buf[128];  //缓冲区
    pipe(p1); //用于进程1从父进程读取数据
    pipe(p2); //用于父进程从进程1读取数据
    if (fork() == 0){
        close(p2[1]);  //关闭了管道p2的写入端
        close(p1[0]);  //关闭管道p1的读取端
        pid = getpid();  //获取子进程的PID
        //从p2读取一个字节并写入p1
        int num_read = read(p2[0], buf, 1);
        if (num_read == 1) {
            printf("%d: received ping\n", pid);
            write(p1[1], buf, 1);
        }
    }
    else{
        close(p1[1]);
        close(p2[0]);
        pid = getpid();
        //将一个字节写入p2
        write(p2[1], buf, 1);
        //从p1读取一个字节到buf
        int num_read = read(p1[0], buf, 1);
        if (num_read == 1){
            printf("%d: received pong\n", pid);
        }
    }
    exit(0);
}
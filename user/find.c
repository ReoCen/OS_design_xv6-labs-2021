#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//接收一个路径字符串，返回文件或目录的名称。
char* fmtname(char *path)
{
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

//在指定路径下查找文件
void find(char *path, char *filename)
{
    char buf[512], *p;
    int fd;   //文件描述符
    struct dirent de;   //用于读取目录项
    struct stat st;     //用于获取文件的属性信息
    if ((fd = open(path, 0)) < 0)      //打开指定路径的文件
    {
        fprintf(2, "find: cannot open %s\n", path);   //打印错误信息并返回
        return;
    }

    if (fstat(fd, &st) < 0)    //获取文件的属性信息
    {
        fprintf(2, "find: cannot stat %s\n", path);   //打印错误信息并返回
        close(fd);
        return;
    }

    switch (st.type)   //根据文件的类型进行处理。
    {
    case T_FILE:  
        //比较文件名和指定的文件名，如果相同，则打印出路径。
        if (strcmp(fmtname(path), filename) == 0)
        {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        //
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)   //路径的长度是否超过了缓冲区的大小
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);      //将路径复制到缓冲区buf中
        p = buf + strlen(buf);  //指定指针p指向缓冲区的末尾
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))   //循环从目录中读取目录项
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)    //获取文件的属性信息
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..") != 0)   //文件名不是.或..
            {
                find(buf, filename);   //继续在该子目录下查找。
            }
        }
        break;
    }
    close(fd);    //关闭文件描述符
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: find [path] [filename]\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*according to usr id to print the message information*/
void msg_show_attr(int msg_id, struct msqid_ds msg_info)
{
    int ret = -1;
    sleep(1);
    ret = msgctl(msg_id, IPC_STAT, &msg_info);
    if(ret == -1)
    {
        perror("get message infromation failed\n");
        return ;
    }

    printf("msg_cbytes: %ld \n", msg_info.__msg_cbytes);
    printf("message queue num: %ld\n", msg_info.msg_qnum);
    printf("message queue max byte: %ld\n", msg_info.msg_qbytes);
    printf("last send message process pid: %d\n", msg_info.msg_lspid);
    printf("last reveive message process pid: %d\n", msg_info.msg_lrpid);
//    printf("last send message time: %d\n", wctomb(&(msg_info.msg_stime)));
//    printf("last receive message time: %d\n", wctomob(&(msg_info.msg_rtime)));
    printf("message usr id: %d\n", msg_info.msg_perm.uid);
    printf("message group id: %d\n", msg_info.msg_perm.gid);
}
int main()
{
    int ret = -1;
    int msg_flags, msg_id;
    key_t key;
    struct msgbuf
    {
        int mtype;
        char mtext[13];
    }; // message buffer structure
    struct msqid_ds msg_info;
    struct msgbuf msg_buf;

    int msg_sflags, msg_rflags;
    char* msgpath = "/ipc/msg/";
    key = ftok(msgpath, 'b');
    if(key != -1)
    {
        printf("create success\n");
    }
    else
    {
        printf("create key failed\n");
    }

    msg_flags = IPC_CREAT | IPC_EXCL;
    msg_id = msgget(key, msg_flags | 0x0666);
    if(msg_id == -1)
    {
        printf("create message failed\n");
        return 0;
    }

    msg_show_attr(msg_id, msg_info);

    msg_sflags = IPC_NOWAIT;
    msg_buf.mtype = 10;
    memcpy(msg_buf.mtext, "test message", sizeof("test message")); // copy string
    ret = msgsnd(msg_id, &msg_buf, sizeof("test message"), msg_sflags);
    if(ret == -1)
    {
        printf("send message failed\n");
    }
    msg_show_attr(msg_id, msg_info);


    msg_rflags = IPC_NOWAIT | MSG_NOERROR;
    ret = msgrcv(msg_id, &msg_buf, 13, 10, msg_rflags);
    if(ret == -1)
    {
        printf("receive messge failed\n");
    }
    else
    {
        printf("receive message success: %d\n", ret);
    }
    msg_show_attr(msg_id, msg_info);
    
    /*set message attribute*/
    msg_info.msg_perm.uid = 8;
    msg_info.msg_perm.gid = 8;
    msg_info.msg_qbytes = 12345;
    ret = msgctl(msg_id, IPC_SET, &msg_info);
    if(ret == -1)
    {
        printf("set message attribute failed\n");
        return 0;
    }
    msg_show_attr(msg_id, msg_info);

    ret = msgctl(msg_id, IPC_RMID, NULL);
    if(ret == -1)
    {
        printf("delete message failed\n");
        return 0;
    }
    return 0;
}

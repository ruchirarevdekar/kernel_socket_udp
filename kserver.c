/*
 * AUTHOR       : RUCHIRA REVDEKAR
 * DATE         : 18-JAN-2021
 * FILE NAME    : kserver.c
 * AIM          : Establishing connection between nodes using kernel socket (UDP)
 */

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/net.h>
#include<linux/socket.h>
#include<linux/in.h>
#include<linux/slab.h>
#include <net/sock.h>
#include<net/inet_common.h>
#include<linux/sched.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>

#define SERVER_PORT 1234
#define size 1024

MODULE_LICENSE("GPL");

struct socket *sock;
/*Module init function*/
int init_module(void) 
{
	struct sockaddr_in *server_addr, *client_addr;
	struct kvec *vec;
	struct msghdr *sock_msg;
	int ret,ret1,i,ops,optval=1,ret2,*buff, *buff1,j=0,sum=0;
	
	buff = (int *)kzalloc((size),GFP_KERNEL);

	/*Creating kernel socket*/
	ret = sock_create_kern(AF_INET, SOCK_DGRAM, 0, &sock);
	if(ret < 0)
	{
		printk(KERN_INFO "Socket creation in server failed\n");
	}
	
	/*Updating kernel structure "struct sockaddr_in"*/	
	server_addr = (struct sockaddr_in *) kzalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
	client_addr = (struct sockaddr_in *) kzalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(SERVER_PORT);
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	
	/*For reusing same port 1234*/
	ops  = kernel_setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *) &optval , sizeof(int));
	
	ret1 = kernel_bind(sock,(struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
	if(ret1 < 0)
	{
		printk(KERN_INFO "bind failed\n");
	}
	/*Receiving data from client after connection establishment*/
	sock_msg = (struct msghdr*) kzalloc(sizeof(struct msghdr),GFP_KERNEL);
	vec = (struct kvec*) kzalloc(sizeof(struct kvec),GFP_KERNEL);

	vec->iov_base=(void *)buff;
	vec->iov_len = size;
	sock_msg->msg_name = client_addr;
	sock_msg->msg_namelen = sizeof(struct sockaddr_in);
	sock_msg->msg_control=NULL;		
	sock_msg->msg_controllen=0;
	sock_msg->msg_flags=0;

	while(sum != size)
	{
		ret2 = kernel_recvmsg(sock,sock_msg,vec,1,(size),0);
		
		if(ret2<0)
		{
			printk(KERN_ALERT "receiption failed\n");
		}
		
		else
		{
			buff1 = buff + sum;
			for (i=j; i<((sum + ret2)/sizeof(int)); i++)
			{
				printk(KERN_INFO "%d \t", *((int *)buff1));
				buff1++;
				j = i;
			}

			sum = sum + ret2;
			vec->iov_base = (void *)(buff+sum);
		}
	}

	printk(KERN_INFO "msg received = %d \n",sum);

	return 0;
}

/*Module exit function*/
void cleanup_module(void)
{
	sock_release(sock);
	printk(KERN_ALERT "Server Exit \n");
}






















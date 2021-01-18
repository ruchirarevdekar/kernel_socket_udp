/*
 * AUTHOR       : RUCHIRA REVDEKAR
 * DATE         : 18-JAN-2021
 * FILE NAME    : kserver.c
 * AIM          : Establishing connection between nodes using kernel socket (UDP)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/net.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/inet_common.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/inet.h>
#define size 1024
#define SERVER_PORT 1234
MODULE_LICENSE("GPL");

struct socket *sock;
/*Module init function*/
int init_module(void)
{
	struct sockaddr_in *server_addr;
	struct kvec *vec;
	struct msghdr *sock_msg;
	int ret, ret1,ret2,*buff,*buff1,i;

	buff = (int *)kzalloc((size),GFP_KERNEL);
	buff1 = buff;
	for (i = 0; i<(size/sizeof(int)); i++)
	{
		*buff1 = i;
		buff1++;
	} 
	 /*Create kernel socket*/
	ret = sock_create_kern(AF_INET, SOCK_DGRAM, 0, &sock);
	if (ret<0)
	{
		printk(KERN_INFO "socket creation in client failed\n");
	}
	
	server_addr = (struct sockaddr_in*) kzalloc(sizeof(struct sockaddr_in),GFP_KERNEL);
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	//server_addr->sin_addr.s_addr = in_aton("10.208.23.240");					/*Server IP Address*/
	server_addr->sin_port = htons(SERVER_PORT);
	
	ret1 = inet_dgram_connect(sock, (struct sockaddr*) server_addr,sizeof(struct sockaddr_in),0);

	if (ret1<0)
	{
		printk(KERN_INFO "connect failed\n");
	}

	/*Sending msg to server after connection establishment*/
	vec=(struct kvec*)kzalloc(sizeof(struct kvec),GFP_KERNEL);
	sock_msg=(struct msghdr*)kzalloc(sizeof(struct msghdr),GFP_KERNEL);

	vec->iov_base = (void *)buff;
	vec->iov_len = size;
	sock_msg->msg_name=NULL;
	sock_msg->msg_namelen=0;
	sock_msg->msg_control=NULL;
	sock_msg->msg_controllen=0;
	
	
	ret2 = kernel_sendmsg(sock,sock_msg,vec,1,(size));
	
	if(ret2<0)
	{
		printk(KERN_INFO "sending failed\n");
	}
	printk(KERN_INFO "msg sent  = %d \n",ret2);
	
	sock_release(sock);	
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Client Exit\n");
}



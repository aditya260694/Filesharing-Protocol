#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

pid_t pid;
void error_msg(const char *s)
{	perror(s);
	exit(1);
}

void client() //acts as sender (need to receiver from here also)
{	int sock, bytes_recieved,fl,newproc;  
	char sent_data[4096], recvd_data[4096];
	struct hostent *host;
	struct sockaddr_in server_addr;  

	host = gethostbyname("127.0.0.1");

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		error_msg("Socket");
	else
		printf("Socket Created!\n");
	
	server_addr.sin_family = AF_INET;     
	server_addr.sin_port = htons(5000);	//connecting to 5001 - other person's server there   
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8); 

	while (connect(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) 
		sleep(1);
	newproc=fork();
	if(newproc==0)	//sender part
	{	while(1)
		{	bzero(sent_data,4096);	
			printf("ENTER YOUR MESSAGE, SIR: ");
			gets(sent_data);	
			if (strcmp(sent_data , "q") == 0 || strcmp(sent_data , "Q") == 0)
			{	fl=send(sock,sent_data,strlen(sent_data),0);
				if (fl<0)
					error_msg("Error in Sending Data on Socket!");
				printf("\nYou Closed The Connection!\n");
				kill(pid,SIGTERM);			
				//close(sock);
				break;
			}
			else
			{	fl=send(sock,sent_data,strlen(sent_data),0);
				if (fl<0)
					error_msg("Error in Sending Data on Socket!");
				
			}
		}
	}
	else	//receiver part
	{	while(1)
		{	bzero(recvd_data,4096);	
			bytes_recieved=recv(sock,recvd_data,4096,0);
			char header[500];
			char name[500];
			if(strcmp(recvd_data,"Download")==0)
			{
				
				bzero(recvd_data,4096);	
				bytes_recieved=recv(sock,recvd_data,4096,0);
				printf("Name :%s\n",recvd_data);
				strcpy(name,recvd_data);
				bzero(recvd_data,4096);	
				bytes_recieved=recv(sock,recvd_data,4096,0);
				printf("Size :%s",recvd_data);
				
				bzero(recvd_data,4096);	
				bytes_recieved=recv(sock,recvd_data,4096,0);
				printf("Timestamp :%s",recvd_data);
				
				bzero(recvd_data,4096);	
				bytes_recieved=recv(sock,recvd_data,4096,0);
				printf("Hash :%s",recvd_data);
					
				FILE *f;
				f=fopen(name,"w");

				
				bzero(recvd_data,4096);	
				bytes_recieved=recv(sock,recvd_data,4096,0);
				
				//printf("%d\n",bytes_recieved);
				while(strcmp(recvd_data,"END OF FILE") != 0)
				{
					fprintf(f,"%s",recvd_data);
					
					bzero(recvd_data,4096);	
					bytes_recieved=recv(sock,recvd_data,4096,0);
					fflush(0);

				}
				//fprintf(f,"%d",EOF);
					bzero(recvd_data,4096);	
						
				fclose(f);
			}
			
			if(strcmp(recvd_data,"FileDownload")==0)
			{
				bzero(sent_data,4096);
				int i=0,fl2=0;
				char name[200];
				for(i=13;recvd_data[i]!='\0';i++)
					name[i-13]=recvd_data[i];
				name[i-13]='\0';
				char size[500],u[500];
				size[0]='\0';
				u[0]='\0';

			
				strcpy(sent_data,"Download");
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
			
				strcpy(sent_data,name);
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
			

				strcpy(size,"stat ");
				strcat(size,name);
				strcat(size," | grep Size | awk '{print $2}' > r.txt");
				system(size);
			
			
				FILE *f;
				f=fopen("r.txt","r");
				fgets(u,sizeof(u),f);
				strcat(sent_data,u);
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
				
				strcpy(size,"stat ");
				strcat(size,name);
				strcat(size," | grep Modify | awk '{print $3}' > r.txt");
				system(size);
			
				f=fopen("r.txt","r");
				fgets(u,sizeof(u),f);
				strcat(sent_data,u);
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
		
			
				strcpy(size,"md5sum ");
				strcat(size,name);
				strcat(size," > r.txt");
				system(size);
			
				f=fopen("r.txt","r");
				fgets(u,sizeof(u),f);
				strcat(sent_data,u);
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
		
			
				FILE *f1;
				f1=fopen(name,"r");
				//printf("lol\n");
				if(f1 == NULL)
				{
					printf("Null\n");
					exit(0);
				}
				int c;
				while((c = fread(sent_data,sizeof(char),4096,f1))>0)
				{
				
					write(sock_client, sent_data,4096);
					bzero(sent_data,4096);
					sent_data[0]='\0';
					fl2=1;
				}
				fclose(f1);
				
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					write(sock_client, sent_data,4096);
				}
				bzero(sent_data,4096);
			}

			printf("\nMESSAGE FOR YOU,SIR: \n%s\n",recvd_data);
		}
	}
	kill(newproc,9);	
	close(sock);
	exit(0);
}

void server() //acts as receiver (need to send from here also)
{	int sock_server,sock_client,bytes_recieved,fl; //2 socket descriptors: 1 for binding to port, other for accepting connection  
	char sent_data [4096] , recvd_data[4096];       
	struct sockaddr_in server_addr,client_addr;    
	int sin_size;

	if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		error_msg("Socket");
	
	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(5001);	//make own server at 5000    
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 

	if (bind(sock_server, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1) 
		error_msg("Unable to bind");
	else
		printf("Server Socket Bound to Port 5000\n");
	
	if (listen(sock_server, 5) == -1) 
		error_msg("Listen");
	else
		printf("Server Waiting for Client on Port 5000\n");
	
	fflush(stdout);
	sin_size = sizeof(struct sockaddr_in);
	sock_client = accept(sock_server, (struct sockaddr *)&client_addr,&sin_size);
	if (sock_client == -1)
		error_msg("Accept");

	while (1)
	{	bzero(recvd_data,4096);	
		bytes_recieved=recv(sock_client,recvd_data,4096,0);
		if (bytes_recieved<0)
			error_msg("Error in Sending Data on Socket!");
		if (strcmp(recvd_data , "q") == 0 || strcmp(recvd_data , "Q") == 0)
		{	printf("\nYOUR MESSAGE IS %s\n",recvd_data); 
			kill(pid,SIGTERM);
			break;
		}
		else if (strncmp(recvd_data , "IndexGet", 8) == 0)
		{	//do processing and send bata back
			bzero(sent_data,4096);	
			if (strncmp(recvd_data , "IndexGet LongList", 17) == 0)
			{	int f=0,fl2=0,x;
				system("bash longlist.sh");
				FILE *fd = fopen("outfile", "r");
				while((f= fread(sent_data, sizeof(char),4096, fd)) > 0)
				{	x=send(sock_client, sent_data,strlen(sent_data),0);
					if (x<0)
						error_msg("Error in Sending Data on Socket!");
					bzero(sent_data, 4096);
					fl2=1;
				}
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					send(sock_client, sent_data,strlen(sent_data),0);
				}
				fclose(fd);
				system("rm outfile");	
			}
			else if (strncmp(recvd_data , "IndexGet ShortList", 18) == 0)	//SHORTLIST WORK
			{	int f=0,l=0,i=19,fl2=0,x;
				char st[200],str[400];
				while(i!=strlen(recvd_data))
					st[l++]=recvd_data[i++];
				st[l]='\0';
				sprintf(str,"bash shortlist.sh %s",st);
				system(str);
				FILE *fd = fopen("outfile", "r");
				while((f= fread(sent_data, sizeof(char),4096, fd)) > 0)
				{	x=send(sock_client, sent_data,strlen(sent_data),0);
					if (x<0)
						error_msg("Error in Sending Data on Socket!");
					bzero(sent_data, 4096);
					fl2=1;
				}
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					send(sock_client, sent_data,strlen(sent_data),0);
				}
				fclose(fd);
				system("rm outfile");	
			}	
			else if (strncmp(recvd_data , "IndexGet RegEx", 14) == 0)	//REGEX WORK
			{	int f=0,k=0,i=15,fl2=0;
				char st[500],str[400];
				while(i!=strlen(recvd_data))
					st[k++]=recvd_data[i++];
				st[k]='\0';
				sprintf(str,"find . -name \"%s\" -exec ls -l {} \\; | awk '{print $9\"\\t\\t\\t\"$5\"\\t\\t\\t\"$6,$7,$8}' | cut -c "" 3- > outfile",st);
				//sprintf(str,"find . -name \"%s\" -exec ls -l {} \\; | awk '{print \"\\nName\\t\\t\\t\\t\\t\\tSize\\t\\t\\tTime\"}{printf \"%-40s\\t%-10s\\t%-6s%-6s%-6s\\n\" ,$9,$5,$6,$7,$8}' > outfile",st);				
				system(str);
				//sprintf(str,"bash regex.sh %s",st);
				//system(str);
				FILE *fd = fopen("outfile", "r");
				while((f= fread(sent_data, sizeof(char),4096, fd)) > 0)
				{	send(sock_client, sent_data,strlen(sent_data),0);
					//error handling for send()
					bzero(sent_data, 4096);
					fl2=1;
				}
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					send(sock_client, sent_data,strlen(sent_data),0);
				}
				fclose(fd);
				system("rm outfile");	
			}		
		}
		else if (strncmp(recvd_data , "FileHash", 8) == 0)
		{	bzero(sent_data,4096);	
			if (strncmp(recvd_data , "FileHash Verify", 15) == 0)
			{	int f=0,k=0,i=16,fl2=0;
				char st[500],str[400];
				while(i!=strlen(recvd_data))
					st[k++]=recvd_data[i++];
				st[k]='\0';
				sprintf(str,"bash verify.sh %s > outfile",st);
				system(str);
				FILE *fd = fopen("outfile", "r");
				while((f= fread(sent_data, sizeof(char),4096, fd)) > 0)
				{	send(sock_client, sent_data,strlen(sent_data),0);
					//error handling for send()
					bzero(sent_data, 4096);
					fl2=1;
				}
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					send(sock_client, sent_data,strlen(sent_data),0);
				}
				fclose(fd);
				system("rm outfile");	
			}
			else if (strncmp(recvd_data , "FileHash CheckAll", 17) == 0)
			{	int f=0,fl2=0;
				system("bash checkall.sh > outfile");
				FILE *fd = fopen("outfile", "r");
				while((f= fread(sent_data, sizeof(char),4096, fd)) > 0)
				{	send(sock_client, sent_data,strlen(sent_data),0);
					//error handling for send()
					bzero(sent_data, 4096);
					fl2=1;
				}
				if(fl2)
				{	strcpy(sent_data,"END OF FILE");	
					send(sock_client, sent_data,strlen(sent_data),0);
				}
				fclose(fd);
				system("rm outfile");	
			}
		}
		else if (strncmp(recvd_data , "FileDownload", 12) == 0)
		{	

			bzero(sent_data,4096);
			int i=0,fl2=0;
			char name[200];
			for(i=13;recvd_data[i]!='\0';i++)
				name[i-13]=recvd_data[i];
			name[i-13]='\0';
			char size[500],u[500];
			size[0]='\0';
			u[0]='\0';

			
			strcpy(sent_data,"Download");
			write(sock_client, sent_data,4096);
			bzero(sent_data,4096);
			
			strcpy(sent_data,name);
			write(sock_client, sent_data,4096);
			bzero(sent_data,4096);
			

			strcpy(size,"stat ");
			strcat(size,name);
			strcat(size," | grep Size | awk '{print $2}' > r.txt");
			system(size);
			
			
			FILE *f;
			f=fopen("r.txt","r");
			fgets(u,sizeof(u),f);
			strcat(sent_data,u);
			write(sock_client, sent_data,4096);
			bzero(sent_data,4096);
				
			strcpy(size,"stat ");
			strcat(size,name);
			strcat(size," | grep Modify | awk '{print $3}' > r.txt");
			system(size);
			
			f=fopen("r.txt","r");
			fgets(u,sizeof(u),f);
			strcat(sent_data,u);
			write(sock_client, sent_data,4096);
			bzero(sent_data,4096);
		
			
			strcpy(size,"md5sum ");
			strcat(size,name);
			strcat(size," > r.txt");
			system(size);
			
			f=fopen("r.txt","r");
			fgets(u,sizeof(u),f);
			strcat(sent_data,u);
			write(sock_client, sent_data,4096);
			bzero(sent_data,4096);
		
			
			FILE *f1;
			f1=fopen(name,"r");
			//printf("lol\n");
			if(f1 == NULL)
			{
				printf("Null\n");
				exit(0);
			}
			int c;
			while((c = fread(sent_data,sizeof(char),4096,f1))>0)
			{
				
				write(sock_client, sent_data,4096);
				bzero(sent_data,4096);
				sent_data[0]='\0';
				fl2=1;
			}
			fclose(f1);
				
			if(fl2)
			{	strcpy(sent_data,"END OF FILE");	
				write(sock_client, sent_data,4096);
			}
			bzero(sent_data,4096);
		}
		else if (strncmp(recvd_data , "FileUpload", 10) == 0)
		{	
			int i;
			char name[300];
			bzero(sent_data,4096);
			for(i=11;recvd_data[i]!='\0';i++)
				name[i-11]=recvd_data[i];

			sprintf(sent_data,"FileDownload %s",name);
			write(sock_client, sent_data,4096);


		}
		else
			printf("\nMESSAGE FOR YOU,SIR: %s\n",recvd_data);
		
		fflush(stdout);
		while(waitpid(-1, NULL, WNOHANG) > 0);	//wait for any child processes
	}
  	close(sock_client);
	printf("\nCONNECTION CLOSED\n");
	close(sock_server);
	exit(0);
} 

int main(int argc, char *argv[])	//connects to 5001, while own server at 5000
{	pid=fork();
	if(pid==0)
		client();
	else if(pid)
		server();

	return 0;
}

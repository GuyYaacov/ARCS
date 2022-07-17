#include "toolbox.h"
#include <tcpsupp.h>
#include <ansi_c.h>
#include <utility.h>
#define MAX_FILE_SIZE 10000
void SendData(unsigned int handle ,int fromwho,int type,int DataLength,char *data);
int GetData(unsigned int handle,int fromwho,int *type,char *data);

void Log(char* data){
	//int len=strlen(data),len2;
	double currDateTime;
	//char finalSendData[35+len];
	char dateTimeBuffer[50];
	int len=strlen(data);
	char finalSendData[50+len];
	//static char *z = NULL; 
	//static struct tm *tm; 
	//static time_t tt;
	//time (&tt);	tm = localtime (&tt);	
	//z = asctime (tm);	
	//len2=strlen(z);	
	//z[len2-1]='\0';
	GetCurrentDateTime (&currDateTime);//get time
	FormatDateTimeString (currDateTime, "%d/%m/%y - %H:%M", dateTimeBuffer, 50);//set time to string
	
	//sprintf (finalSendData, "%s >>> %s\n",z,data);
	sprintf (finalSendData, "%s:\t%s\n",dateTimeBuffer,data);
	
	char dirname[MAX_PATHNAME_LEN];
	int v;
	v=GetProjectDir (dirname);
	if(v==0) SetDir (dirname);
	FILE *fp;
	fp=fopen("ConnectionLog.txt","a");	
	fprintf(fp,finalSendData);	
	fclose(fp);
}



void WhatIsMyIp(char *ip)
{
	system("cmd.exe /c curl ifconfig.me > myip.txt");
	FILE *fp;
	char line[50];
	int file_exists;
	file_exists = FileExists ("myip.txt", NULL);
	if(file_exists == 1)
	{
		char dirname[MAX_PATHNAME_LEN];
		int v;
		v=GetProjectDir (dirname);
		if(v==0) SetDir (dirname);
		fp = fopen ("myip.txt", "r");
		while(fgets(line,49,fp) != 0)
		{
			sscanf(line,"%s",ip);
		}
		fclose(fp);
		remove ("myip.txt");

	}
}


int SendFile(unsigned int handle ,int fromwho ,char *filename,char *filepath, int new_file_type_const)
{
	char line[MAX_FILE_SIZE];
	SendData(handle ,fromwho,new_file_type_const,strlen(filename),filename);
	FILE *f;
	DisableBreakOnLibraryErrors ();
	f = fopen (filepath, "r");
	if(f == NULL)
	{
		EnableBreakOnLibraryErrors();
		return 0;
	}
	while(fgets(line, 900, f))
	{
		ClientTCPWrite(handle,line,strlen(line),0);
	}
	EnableBreakOnLibraryErrors();
	return 1;
}


int GetFile(unsigned int handle,int new_file_type_const)
{
	int type=0,bytesRead,fileOpenFlag=0;
	char stringReceived[MAX_FILE_SIZE],dateTimeBuffer[50];
	double currDateTime;
	char filename[200]="";
	FILE *f=NULL;
	DisableBreakOnLibraryErrors ();
	while (1)
	{
		bytesRead=ServerTCPRead (handle, stringReceived, MAX_FILE_SIZE, 0);
		sscanf (stringReceived, "%d", &type);
		stringReceived[bytesRead]='\0';
		if (bytesRead > 0)
		{
			if(type == new_file_type_const && fileOpenFlag == 0)
			{
				GetCurrentDateTime (&currDateTime);//get time
				FormatDateTimeString (currDateTime, "%d/%m/%y_%H:%M", dateTimeBuffer, 50);//set time to string
				sprintf (filename, "%s.ino",stringReceived+6);
				f = fopen (filename, "w");
				fileOpenFlag = FileExists (filename, NULL);
				if (fileOpenFlag!=1)
				{
					EnableBreakOnLibraryErrors (); 
					return 0;
				}
			}
			else if(fileOpenFlag == 1)
			{
				fputs (stringReceived, f);
			}
			
			else
			{
				if(fileOpenFlag == 1)
				{
					fclose(f);
					remove(filename);
				}
				EnableBreakOnLibraryErrors ();
				return 0;
			}
		}
		else
		{
				fclose(f);
				EnableBreakOnLibraryErrors ();
				return 1;
		}
	}
	/* Enable library error checking. */
		
}


/*int OpenConnection (int vFromWho, char *vIp, unsigned int vPort, char *vUserName, char *vPassword)
{
	char sStr[50];
	int iReturnCode;
	unsigned int iHandle;
	
	if (vFromWho < 1) 
	{ // Client connection
		iReturnCode = ConnectToTCPServer (&iHandle, vPort, vIp, ClientFunction, 0, 0); 
		if (iReturnCode == 0)
		{
			sprintf(sStr, "Client %s connected to server (ip = %s, port = %d)", vUserName, vIp, vPort);
			Log (sStr);
			sprintf (sStr, "%s,%s", vUserName, vPassword);
			SendData (iHandle, vFromWho, LOG_IN, strlen(sStr), sStr);
			sprintf(sStr, "Client %s sent login request to server)", vUserName);
		}
		else
		{
		   iHandle = iReturnCode * -1;
			sprintf(sStr, "Client %s failed to connect to server (ip = %s, port = %d)", vUserName, vIp, vPort);
		}
	}
	else
	{ // Server connection   
		iReturnCode = RegisterTCPServer(vPort, ServerFunction, 0);
		if (iReturnCode == 0)
			sprintf(sStr, "Server has been registered (port = %d)", vPort);
		else
			sprintf(sStr, "Server failed to register (port = %d)", vPort);
	   iHandle = iReturnCode * -1;
	}
	Log (sStr);
	return iHandle;
}

void CloseConnection (int vFromWho, int vHandleOrPort)
{
	char sStr[50];
	
	if (vFromWho < 1)
	{ // Client close connection
		DisconnectFromTCPServer (vHandleOrPort);  
		sprintf(sStr, "Client disconnected (handle = %d)", vHandleOrPort);
	}
	else
	{ // Server close connection
		UnregisterTCPServer (vHandleOrPort);	 
		sprintf(sStr, "Server unregistered (ip = %d)", vHandleOrPort);
	}
	Log (sStr);
}*/


void SendData(unsigned int handle ,int fromwho,int type,int DataLength,char *data){
	char str[10000];
	sprintf(str,"%d %d %s",type,DataLength,data);
	if(fromwho==0)
		ClientTCPWrite(handle,str,strlen(str),0);
	else
		ServerTCPWrite(handle,str,strlen(str),0);
}


int GetData(unsigned int handle,int fromwho,int *type,char *data)
{
 	char stringReceived[10000];
	int btread;
	int len=0;
	if(fromwho>0)
		btread=ServerTCPRead (handle, stringReceived, 10000, 0); 
	else
		btread=ClientTCPRead(handle, stringReceived, 10000, 0);
	stringReceived[btread]='\0';
	sscanf (stringReceived, "%d %d %[^\t]", type,&len,data);
	data[len]='\0';
	return len;		
}


int Burn(char *filename,int com)
{
	char commend[1000],folder[1000],foldername[500],disk[3];
	sscanf(filename,"%[^.]",foldername);
	int result;
	FILE *f;
	remove("log.txt");
	GetFullPathFromProject (foldername, folder);
	sscanf(folder,"%[^\\]",disk);
	DisableBreakOnLibraryErrors ();
	result = MakeDir (folder);
	if (result != -9 && result != 0)
	{
		EnableBreakOnLibraryErrors();
		return 0;
	}
	CopyFile (filename, folder);
	sprintf (commend, "cmd.exe /c arduino_debug  --port COM%d --upload %s\\%s & >log.txt call echo %%^errorlevel%%",com,foldername,filename);
	system (commend);
	int file_exists;
	file_exists = FileExists ("log.txt", NULL);
	while(file_exists != 1) 
		file_exists = FileExists ("log.txt", NULL);
	if(file_exists==1)
	{
		f = fopen ("log.txt", "r");
		fscanf (f, "%d", &result);
		fclose(f);
		if(result == 0)
		{
			result = 1;
		}
		else
		{
			result = 0;
		}
		remove("log.txt");
		sprintf(folder,"%s\\%s",foldername,filename);
		remove(folder);
		DeleteDir (foldername);
		EnableBreakOnLibraryErrors();
		return result;
	}
	return 0;
}

int Authentication(char username[],char password[])
{
	FILE *fp;
	char line[50],name_from_the_table[25],password_from_the_table[25];
	int val = 0,file_exists;
	file_exists = FileExists ("Whitelist.CSV", NULL);														  
	if(file_exists != 1)
		return 2;
	fp = fopen ("Whitelist.CSV", "r");																		   
	while(fgets(line,49,fp) != 0)																		  
	{
		sscanf(line,"%[^,],%s",name_from_the_table,password_from_the_table);

		if(strcmp(name_from_the_table,username) == 0 && strcmp(password_from_the_table,password) == 0)	  
		{
			val = 1;																					 
			break;
		}
	}
	fclose(fp);
	return val;
}

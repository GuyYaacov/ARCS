#include <cvintwrk.h>
#include "toolbox.h"
#include <utility.h>
#include <tcpsupp.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "Arduino Remote Contro System.h"
#include "actions.h"
#include "ARS.h"
#define NOT_AUT_USER "@$@#%$#%#$"
//*********************************************************************************************************************
void FileSelection(int state);
int ClientFunction (unsigned handle, int event, int errCode, void *callbackData);
int ServerFunction (unsigned handle, int event, int errCode, void *callbackData);
static int panelHandle, panelHandle3, panelHandle2, panelHandle4; 
void ServerState(int state);
void ClientState(int state);
void ServerEstablished(int state);
void BurnState(int burn);
//*********************************************************************************************************************
int client_established_flag=0;
int server_established_flag=0;
int a_client_is_inside_flag=0;
int i_am_attempt_to_connect_flag=0;
int show_me_what_you_got=0;
int burning_position_flag=0;
int i_am_in_flag=0;
int sa_scroll_flag=0, ca_scroll_flag=0;
int authentication_time_counter=0;
int sending_ard_data_flag=0;
int client_selecting_file_flag=0,client_burn_flag=0;
unsigned int arduino_comm_port=3;
unsigned int server_port=8887;
unsigned int client_con_handle;
unsigned int a_client_handle, go_away_handle;
char my_ip_address[16]="000.000.000.000";
char server_ip_address[16]="000.000.000.000"; 
char client_ip_address[16]="000.000.000.000";
char connected_client_ip_address[16]="000.000.000.000";
char username[10]={'Username'};
char password[9]={'Password'};
char username_sent[16]=NOT_AUT_USER,password_sent[9];
char ino_file_path[2000];
char filename_from_user[200];
char filepath[MAX_PATHNAME_LEN ];
//*********************************************************************************************************************


void PutShowLast(int fromwho,char *msg)
{
	int lines;
	int FirstLine;
	int VisibleLines;
	if(fromwho>0)
	{
		InsertTextBoxLine (panelHandle2, PANEL_2_TEXTBOX, -1, msg);
		GetCtrlAttribute (panelHandle2, PANEL_2_TEXTBOX, ATTR_VISIBLE_LINES, &VisibleLines);
		GetNumTextBoxLines(panelHandle2, PANEL_2_TEXTBOX,&lines);
	}
	else
	{
		InsertTextBoxLine (panelHandle, PANEL_TEXTBOX_CLIENT, -1, msg);
		GetCtrlAttribute (panelHandle, PANEL_TEXTBOX_CLIENT, ATTR_VISIBLE_LINES, &VisibleLines);
		GetNumTextBoxLines(panelHandle, PANEL_TEXTBOX_CLIENT,&lines);

	}
	FirstLine=0;
	if (lines>VisibleLines)
	{
		FirstLine=lines-VisibleLines;
	}
	if(fromwho>0 && sa_scroll_flag>0) SetCtrlAttribute (panelHandle2, PANEL_2_TEXTBOX, ATTR_FIRST_VISIBLE_LINE, FirstLine);
	else if (fromwho == 0 && ca_scroll_flag>0) SetCtrlAttribute (panelHandle, PANEL_TEXTBOX_CLIENT, ATTR_FIRST_VISIBLE_LINE, FirstLine);
}



void CVICALLBACK SerialFunc (int portNumber, int eventMask, void *callbackData)
{
	char ard_str[256]=">> ";
	int bytesRead;
	if(a_client_is_inside_flag==1 && client_selecting_file_flag==0 &&burning_position_flag==0 && sending_ard_data_flag==0){
		SendData(a_client_handle,SERVER,ARDUINO_DATA,1,"v"); 
		sending_ard_data_flag=1;
	}
	while(GetInQLen (arduino_comm_port)>0)
	{
		bytesRead=ComRdTerm (arduino_comm_port, ard_str+3, 253, '\n');
		if (bytesRead>0)ard_str[bytesRead-1+3]='\0';
			if(a_client_is_inside_flag==1 && burning_position_flag==0 && client_selecting_file_flag==0 && sending_ard_data_flag==1)
				ServerTCPWrite(a_client_handle,ard_str,strlen(ard_str),0);
			if(show_me_what_you_got==1)
				PutShowLast(SERVER,ard_str);
	}
}


int ClientFunction (unsigned handle, int event, int errCode, void *callbackData)
{
	char msg[2500];
	switch(event)
	{
		case TCP_DISCONNECT:
			client_established_flag=0;
			ClientState(0);
			BurnState(0);
			break;
		case TCP_DATAREADY:
			char data[256];
			int action;
			int len;
			len=GetData(handle,CLIENT,&action,data);
ACTION: switch(action)
			{
				case LOG_IN_RES:
					ClientState(1);
					BurnState(0);
					sprintf(msg,"Connected to: %s",server_ip_address);
					Log(msg);
					PutShowLast(CLIENT,msg);
					client_established_flag=1;
					break;
				case ERROR:
					PutShowLast(CLIENT,data);
					Log(data);
					break;
				case ARDUINO_DATA:
					DisableBreakOnLibraryErrors();
					while(client_burn_flag==0)
					{
						int size;
						char ard_str[10000];
						size=ClientTCPRead(handle,ard_str,10000,0);
						if(size > 0)ard_str[size]='\0';
						if (ard_str[0] != '>')
						{
							sscanf (ard_str, "%d %d %[^\t]", &action,&len,data);
							data[len]='\0';
							if (action != GOOD_TO_GO) SendData(client_con_handle,CLIENT,ARDUINO_STOP_DATA,1,"v");
							EnableBreakOnLibraryErrors();
							goto ACTION;
						}
						else if (size<0){
							SendData(client_con_handle,CLIENT,ARDUINO_STOP_DATA,1,"v");
							break;
						}
						PutShowLast(CLIENT,ard_str);
						Log(ard_str);
					}
						EnableBreakOnLibraryErrors(); 
					break;
				case GOOD_TO_GO:
					sprintf(msg,"Attempting to burn: %s",ino_file_path);
					Log(msg);
					PutShowLast(CLIENT,msg);
					SendFile(client_con_handle,CLIENT,username,ino_file_path,CREATE_FILE_NAME);
					break;
				case BURN_OK:
					sprintf(msg,"File was burned successfully.");
					Log(msg);
					PutShowLast(CLIENT,msg);
					ClientState(1);
					BurnState(0);
					FileSelection(0);
					client_burn_flag=0;
					break;
				case BURN_NOT_OK:
					sprintf(msg,"Burning process has failed.");
					Log(msg);
					PutShowLast(CLIENT,msg);
					ClientState(1);
					BurnState(0);
					FileSelection(0);
					client_burn_flag=0;
					break;
			}
			
			break;
	}
	return 0;	
}


int ServerFunction (unsigned handle, int event, int errCode, void *callbackData)
{
	char msg[1000];
	switch(event){
		case TCP_CONNECT:
			if(a_client_is_inside_flag!=0 || burning_position_flag==1 ){
				go_away_handle=handle;
				SetCtrlAttribute (panelHandle2, PANEL_2_AUT_TIMER, ATTR_ENABLED, 1);
			}
			else if(a_client_is_inside_flag==0 && burning_position_flag==0 ){
				a_client_handle=handle;
				SetCtrlAttribute (panelHandle2, PANEL_2_AUT_TIMER, ATTR_ENABLED, 1);
			}
			break;
		case TCP_DISCONNECT:
			if(handle==a_client_handle)
			{
				a_client_is_inside_flag=0;
				sprintf(msg,"%s was disconnected.",client_ip_address);
				PutShowLast(SERVER,msg);
				Log(msg);
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_USERNAME_STR, "");
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_IP_ADDR_STR, "0.0.0.0");
				SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 1);
				ServerState(1);
				ServerEstablished(1);
				sprintf(username_sent,"%s",NOT_AUT_USER);
				sending_ard_data_flag=0;
				client_selecting_file_flag=0;
			}
			break;
		case TCP_DATAREADY:
			char data[256]="";
			int action=0;
			int len;
			len=GetData(handle,SERVER,&action,data);
			switch(action)
			{
				case LOG_IN:
					sscanf (data, "%[^,],%[^,],%s", username_sent,password_sent,client_ip_address);
					sprintf(msg,"%s is trying to connect...",client_ip_address);
					PutShowLast(SERVER,msg);
					Log(msg);
					if(a_client_is_inside_flag!=0)
					{
						sprintf(data,"Server is busy. Only a single client can be connected.");
						SendData(go_away_handle,SERVER,ERROR,strlen(data),data);
						sprintf(msg,"%s was kicked because %s is already connected.",client_ip_address,connected_client_ip_address);
						PutShowLast(SERVER,msg);
						Log(msg);
						DisconnectTCPClient (go_away_handle);
					}
					else
					{
						int answear;
						answear=Authentication(username_sent,password_sent);
						if(answear==1){
							a_client_is_inside_flag=1;
							SetCtrlVal (panelHandle2, PANEL_2_CLIENT_USERNAME_STR, username_sent);
							SetCtrlVal (panelHandle2, PANEL_2_CLIENT_IP_ADDR_STR, client_ip_address);
							sprintf(msg,"%s is connected.",client_ip_address);
							PutShowLast(SERVER,msg);
							Log(msg);
							sprintf(msg,"1");
							SendData(a_client_handle,SERVER,LOG_IN_RES,strlen(msg),msg);
							sprintf(connected_client_ip_address,"%s",client_ip_address);
							ServerState(2);
							ServerEstablished(3);
						}
						else if(answear==0)
						{
							sprintf(data,"Connection failed due to an invalid username or password.");
							SendData(a_client_handle,SERVER,ERROR,strlen(data),data);
							DisconnectTCPClient (a_client_handle);
							sprintf(msg,"%s was kicked due to an invalid username or password.",client_ip_address);
							Log(msg);
							PutShowLast(SERVER,msg);
						}
						else if(answear==2)
						{
							MessagePopup ("Error", "Cannot find Whitelist.csv");
							DisconnectTCPClient (a_client_handle);
							sprintf(msg,"%s is kicked. Cannot find Whitelist.csv",client_ip_address);
							Log(msg);
							PutShowLast(SERVER,msg);
							sprintf(msg,"Cannot connect. Unknown Error.");
							SendData(a_client_handle,SERVER,ERROR,strlen(msg),msg); 
						}
					}
					SetCtrlAttribute (panelHandle2, PANEL_2_AUT_TIMER, ATTR_ENABLED, 0);
					authentication_time_counter=0;
					break;
				case COMMAND:
					ComWrt (arduino_comm_port, data, strlen(data));
					sprintf(msg,">>> %s: %s",username_sent,data);
					Log(msg);
					PutShowLast(SERVER,msg);
					break;
				case START_GET_FILE_RUTINE:
					CloseCom (arduino_comm_port); //////////////////////////////////////// 
					int burn_flag;
					burning_position_flag=1;
					sending_ard_data_flag=0;
					SendData(a_client_handle,SERVER,GOOD_TO_GO,1,"v");
					char inofile[200];
					sprintf(inofile,"%s.ino",username_sent);
					sprintf(msg,"%s was recieved from a client. Attempting to burn.",inofile);
					Log(msg);
					PutShowLast(SERVER,msg);
					burn_flag=GetFile(a_client_handle,CREATE_FILE_NAME);
					if (burn_flag)
					{
						SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_DIMMED, 1);
						//CloseCom (arduino_comm_port);
						burn_flag=Burn(inofile,arduino_comm_port);
						if (burn_flag==1)
						{
							SendData(a_client_handle,SERVER,BURN_OK,1,"v");
							sprintf(msg,"%s was successfully burned onto Arduino.",inofile);
						}
						else
						{
							SendData(a_client_handle,SERVER,BURN_NOT_OK,1,"v");
							sprintf(msg,"Burning of %s has failed.",inofile);
						}
						PutShowLast(SERVER,msg);
						//OpenComConfig (arduino_comm_port, "", 9600, 0, 8, 1, 512, 512);
						//FlushInQ (arduino_comm_port);
						//InstallComCallback (arduino_comm_port, LWRS_RXFLAG, 0, '\n', SerialFunc, 0);
						//SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_DIMMED, 0);
						burning_position_flag=0;

					}
					else
					{
						SendData(a_client_handle,SERVER,BURN_NOT_OK,1,"v");
						sprintf(msg,"Failed to proccess the sketch file.");
						PutShowLast(SERVER,msg);
						Log(msg);
						burning_position_flag=0;
					}
					OpenComConfig (arduino_comm_port, "", 9600, 0, 8, 1, 512, 512);
					FlushInQ (arduino_comm_port);
					InstallComCallback (arduino_comm_port, LWRS_RXFLAG, 0, '\n', SerialFunc, 0);
					SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_DIMMED, 0);

					break;
				case RESET_ARDUINO:
					SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_DIMMED, 1);
					sprintf(msg,"%s has restarted the Arduino.",username_sent);
					Log(msg);
					PutShowLast(SERVER,msg);
					CloseCom (arduino_comm_port);
					OpenComConfig (arduino_comm_port, "", 9600, 0, 8, 1, 512, 512);
					FlushInQ (arduino_comm_port);
					InstallComCallback (arduino_comm_port, LWRS_RXFLAG, 0, '\n', SerialFunc, 0);
					SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_DIMMED, 0);
					break;
				case ARDUINO_STOP_DATA:
					sending_ard_data_flag=0; 
					break;
				case START_FILE_SELECTION:
					client_selecting_file_flag=1;
					break;
				case END_FILE_SELECTION:
					client_selecting_file_flag=0;
					break;
					
			}
			break;
	}
	return 0;
}

void ClientState(int state)
{
	switch(state)
	{
		case 0: // not conected
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_FILE_SELECT_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CRESET_ARD_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_SEND_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_SERVER_IP, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_TCP_PORT, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_STRING_USERNAME, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_STRING_PASSWORD, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 1);
			SetCtrlVal (panelHandle, PANEL_CLIENT_STATUS_LED, 0);
			SetCtrlAttribute (panelHandle, PANEL_CONNECT_BT, ATTR_LABEL_TEXT, "Connect");
			break;

		case 1: // connected
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_FILE_SELECT_BT, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CRESET_ARD_BT, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_SEND_BT, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_SERVER_IP, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_TCP_PORT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_STRING_USERNAME, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_STRING_PASSWORD, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 0);
			SetCtrlVal (panelHandle, PANEL_CLIENT_STATUS_LED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CONNECT_BT, ATTR_LABEL_TEXT, "Disconnect");
			break;
			
	}
}
void BurnState(int burn)
{
	switch(burn)
	{
		case 1:// burning
			SetCtrlAttribute (panelHandle, PANEL_SEND_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_FILE_SELECT_BT, ATTR_DIMMED, 1);
			break;
		case 0://not burning
			SetCtrlAttribute (panelHandle, PANEL_SEND_BT, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_COMMAND_LINE, ATTR_DIMMED, 0);
			if (client_established_flag==1)SetCtrlAttribute (panelHandle, PANEL_FILE_SELECT_BT, ATTR_DIMMED, 0);
			break;
	}

}

void ServerState(int state){
	switch(state){
		case 0: // not established
		SetCtrlAttribute (panelHandle2, PANEL_2_SRESET_ARD_BT, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle2, PANEL_2_ARDUINO_PORT, ATTR_DIMMED, 0); 
		SetCtrlAttribute (panelHandle2, PANEL_2_TCP_PORT, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 1);
		SetCtrlVal(panelHandle2, PANEL_2_SERVER_STATUS_LED ,0);
		SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_LABEL_TEXT, "Create Server");
		break;
		
		case 1: // established no client connected
		SetCtrlAttribute (panelHandle2, PANEL_2_SRESET_ARD_BT, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle2, PANEL_2_ARDUINO_PORT, ATTR_DIMMED, 1); 
		SetCtrlAttribute (panelHandle2, PANEL_2_TCP_PORT, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle2, PANEL_2_SERVER_STATUS_LED, ATTR_ON_COLOR, MakeColor(255, 204, 0)); 
		SetCtrlVal(panelHandle2, PANEL_2_SERVER_STATUS_LED ,1);
		SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_LABEL_TEXT, "Shutdown Server");
		break;
		case 2: // established client connected
		SetCtrlAttribute (panelHandle2, PANEL_2_SRESET_ARD_BT, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle2, PANEL_2_ARDUINO_PORT, ATTR_DIMMED, 1); 
		SetCtrlAttribute (panelHandle2, PANEL_2_TCP_PORT, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle2, PANEL_2_SERVER_STATUS_LED, ATTR_ON_COLOR, MakeColor(128, 255, 0)); 
		SetCtrlVal(panelHandle2, PANEL_2_SERVER_STATUS_LED ,1);
		SetCtrlAttribute (panelHandle2, PANEL_2_CREATE_SERVER, ATTR_LABEL_TEXT, "Shutdown Server");
		break;
	}
}

void ServerEstablished(int state){
	switch(state){
		case 0://not established
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_CTRL_VAL, "Server is not established.");
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_TEXT_COLOR, VAL_RED);
			break;
		case 1://established , no client 
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_CTRL_VAL, "Server established. Waiting for connection.");
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_TEXT_COLOR, MakeColor(255, 153, 51));
			break;
		case 2://try to establish 
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_CTRL_VAL, "Trying to establish a server...");
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_TEXT_COLOR, MakeColor(255, 153, 51));
			break;
		case 3://establish and client is connected
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_CTRL_VAL, "Server established. Client connected.");
			SetCtrlAttribute (panelHandle2, PANEL_2_ESTABLISHED_MSG, ATTR_TEXT_COLOR, VAL_DK_GREEN);
			break;
			
		
	}
}
void FileSelection(int state){
	char msg[250];
	switch(state){
		case 0://not selected.
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_CTRL_VAL, "No selected file.");
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_TEXT_COLOR, VAL_RED);
			SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CONNECT_BT, ATTR_DIMMED, 0);
			break;
		case 1://selected
			char *name;
			name=strrchr(filepath,'\\');
			sprintf(msg,"%s selected.",name+1);
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_CTRL_VAL, msg);
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_TEXT_COLOR, VAL_BLUE);
			SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 0);
			break;
		case 2://sent to burn
			char *fname;
			fname=strrchr(filepath,'\\');
			char fmsg[250];
			sprintf(fmsg,"%s is sent to the server for burning.",fname+1);
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_CTRL_VAL, fmsg);
			SetCtrlAttribute (panelHandle, PANEL_FILE_MSG, ATTR_TEXT_COLOR, VAL_DK_GREEN);
			SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CONNECT_BT, ATTR_DIMMED, 1);
			break;
			
	}
}

void ProgramStatesMachine(int ProgramState)
{
	switch(ProgramState){
		
		case 0:// CLIENT 
			HidePanel (panelHandle3);
			DisplayPanel (panelHandle);
			ClientState(0);
			BurnState(0);
			break;
		case 1: // SERVER
			HidePanel (panelHandle3);
			DisplayPanel (panelHandle2);
			ServerState(0);
			break;
		case 2: // choosing position
			HidePanel (panelHandle);
			HidePanel (panelHandle2);
			ClientState(0);
			BurnState(0);
			ServerState(0);
			ServerEstablished(0);
			DisplayPanel (panelHandle3);
			break;
	}
}

void init(){
	int x,y;
	int top,left;
	int xw,yw;
	GetRelativeMouseState (panelHandle3, 0, &x, &y, NULL, NULL, NULL);
	GetPanelAttribute (panelHandle3, ATTR_LEFT, &xw);
	GetPanelAttribute (panelHandle3, ATTR_TOP, &yw);
	top=yw+y;
	left=xw+x;
	SetPanelAttribute (panelHandle3, ATTR_LEFT, left);
	SetPanelAttribute (panelHandle3, ATTR_TOP, top);
	
}
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Arduino Remote Contro System.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle3 = LoadPanel (0, "Arduino Remote Contro System.uir", PANEL_3)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "Arduino Remote Contro System.uir", PANEL_2)) < 0)
		return -1;
	if ((panelHandle4 = LoadPanel (0, "Arduino Remote Contro System.uir", PANEL_4)) < 0)
		return -1;
	
	DisplayPanel (panelHandle3);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	DiscardPanel (panelHandle3);
	DiscardPanel (panelHandle2);
	DiscardPanel (panelHandle4);
	return 0;
}

int CVICALLBACK exit_client (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK ExitFromAll (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK exit_server (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			if(server_established_flag==1){
				UnregisterTCPServer (server_port);
				ServerEstablished(0);
				CloseCom (arduino_comm_port);
			}
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK ConnectToServer (int panel, int control, int event,void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (client_established_flag==0)
			{
				int result;
				GetCtrlVal (panelHandle, PANEL_STRING_USERNAME, username);
				GetCtrlVal (panelHandle, PANEL_STRING_PASSWORD, password);
				GetCtrlVal (panelHandle, PANEL_SERVER_IP, server_ip_address);
				GetCtrlVal (panelHandle, PANEL_TCP_PORT, &server_port);
				DisableBreakOnLibraryErrors ();
				result=ConnectToTCPServer (&client_con_handle, server_port, server_ip_address,ClientFunction , 0, 0);
				if(result==0)
				{
					char data[256];
					sprintf(data,"%s,%s,%s",username,password,my_ip_address);
					SendData(client_con_handle,CLIENT,LOG_IN,strlen(data),data);
				}
				EnableBreakOnLibraryErrors();
			}
			else
			{
				DisconnectFromTCPServer (client_con_handle);
				client_established_flag=0;
				ClientState(0);
				BurnState(0);
			}
			break;
	}
	return 0;
}

int CVICALLBACK BackToMain (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			ProgramStatesMachine(2);
			SetCtrlVal (panelHandle2, PANEL_2_ARDUINO_PORT, arduino_comm_port);
			server_port=8887;
			SetCtrlVal (panelHandle2, PANEL_2_TCP_PORT, server_port);
			SetCtrlVal (panelHandle, PANEL_TCP_PORT, server_port);
			char defulte_ip[16]="0.0.0.0";
			sprintf(server_ip_address,"%s",defulte_ip);
			char defulte_username[10]= "Username";
			char defulte_password[9]= "Password";
			sprintf(username,"%s",defulte_username);
			sprintf(password,"%s",defulte_password);
			SetCtrlVal (panelHandle, PANEL_SERVER_IP, server_ip_address);
			SetCtrlVal (panelHandle, PANEL_STRING_USERNAME, username);
			SetCtrlVal (panelHandle, PANEL_STRING_PASSWORD, password);
			ClientState(0);
			BurnState(0);
			FileSelection(0);
			ServerState(0);
			if(server_established_flag==1)
			{
				UnregisterTCPServer (server_port);
				ServerEstablished(0);
				ServerState(0);
				CloseCom (arduino_comm_port);
				arduino_comm_port=3;
				server_established_flag=0;
			}
			if(client_established_flag==1) DisconnectFromTCPServer (client_con_handle);
			int top,left;
			GetPanelAttribute (panelHandle3, ATTR_LEFT, &left);
			GetPanelAttribute (panelHandle3, ATTR_TOP, &top);
			SetPanelAttribute (panelHandle, ATTR_LEFT, left);
			SetPanelAttribute (panelHandle, ATTR_TOP, top);
			SetPanelAttribute (panelHandle2, ATTR_LEFT, left);
			SetPanelAttribute (panelHandle2, ATTR_TOP, top);
			break;
	}
	return 0;
}

int CVICALLBACK SendAndBurn (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			ClientState(1);
			BurnState(1);
			FileSelection(2);
			client_burn_flag=1;
			SendData(client_con_handle,CLIENT,START_GET_FILE_RUTINE,1,"v");
			break;
	}
	return 0;
}

int CVICALLBACK SendCommand (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char cmmnd[20];
			GetCtrlVal (panelHandle, PANEL_COMMAND_LINE, cmmnd);
			if (strlen(cmmnd)>0) SendData(client_con_handle,CLIENT,COMMAND,strlen(cmmnd),cmmnd);
			SetCtrlVal (panelHandle, PANEL_COMMAND_LINE, "");
			break;
	}
	return 0;
}


int CVICALLBACK SelectInoFile (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
					int file_status;
					SendData(a_client_handle,CLIENT,START_FILE_SELECTION,1,"V");
					file_status = FileSelectPopup ("", "*.ino", "Arduino script file", "Select .ino file", VAL_SELECT_BUTTON, 0, 1, 1, 0, filepath);
					switch(file_status){
						case 0:
							MessagePopup ("No Sketch File Selected", "Please select *.ino file you would like to burn.");
							FileSelection(0);
							SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 1);
							
							break;
						case 1:
							sprintf(ino_file_path,"%s",filepath);
							SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 0);
							FileSelection(1);
							break;
						case 2:
							sprintf(ino_file_path,"%s",filepath);
							SetCtrlAttribute (panelHandle, PANEL_BURN_BT, ATTR_DIMMED, 0);
							FileSelection(1);
							break;
					}
					SendData(a_client_handle,CLIENT,END_FILE_SELECTION,1,"V");

			break;
	}
	return 0;
}

int CVICALLBACK ServerOrClientSelection (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{

		case EVENT_COMMIT:
			WhatIsMyIp(my_ip_address);
			int top,left;
			GetPanelAttribute (panelHandle3, ATTR_LEFT, &left);
			GetPanelAttribute (panelHandle3, ATTR_TOP, &top);
			SetPanelAttribute (panelHandle, ATTR_LEFT, left);
			SetPanelAttribute (panelHandle, ATTR_TOP, top);
			SetPanelAttribute (panelHandle2, ATTR_LEFT, left);
			SetPanelAttribute (panelHandle2, ATTR_TOP, top);
			switch(control)
			{
				case PANEL_3_CLIENT_BT:
					ProgramStatesMachine(0);
					ClientState(0);
					BurnState(0);
					DeleteTextBoxLines (panelHandle, PANEL_TEXTBOX_CLIENT, 0, -1);

					break;
				case PANEL_3_SERVER_BT:
					ProgramStatesMachine(1);
					ServerState(0);
					ServerEstablished(0);
					SetCtrlVal (panelHandle2, PANEL_2_MY_IP_ADDR_STR, my_ip_address);
					DeleteTextBoxLines (panelHandle2, PANEL_2_TEXTBOX, 0, -1);
					break;
			}
			break;
	}
	return 0;
}

int CVICALLBACK KickClient (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char msg[200];
			sprintf(msg,"%s: %s was kicked out.",username_sent,connected_client_ip_address);
			Log(msg);
			PutShowLast(SERVER,msg);
			sprintf(msg,"You had been kicked out by the server admin.");
			SendData(a_client_handle,SERVER,ERROR,strlen(msg),msg);
			DisconnectTCPClient (a_client_handle);
			a_client_is_inside_flag=0;
			SetCtrlVal (panelHandle2, PANEL_2_CLIENT_USERNAME_STR, "");
			SetCtrlVal (panelHandle2, PANEL_2_CLIENT_IP_ADDR_STR, "0.0.0.0");
			SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 1);
			ServerState(1);
			ServerEstablished(1); 
			sprintf(username_sent,"%s",NOT_AUT_USER);
			sending_ard_data_flag=0;
			client_selecting_file_flag=0;
			break;
	}
	return 0;
}


int CVICALLBACK CreateServer (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			if(server_established_flag==0)
			{
			ServerEstablished(2);
			GetCtrlVal (panelHandle2, PANEL_2_TCP_PORT, &server_port);
			GetCtrlVal (panelHandle2, PANEL_2_ARDUINO_PORT, &arduino_comm_port);
			int x,y=0;
			DisableBreakOnLibraryErrors ();
			x=RegisterTCPServer (server_port, ServerFunction, 0);
			y=OpenComConfig (arduino_comm_port, "", 9600, 0, 8, 1, 512, 512);
			FlushInQ (arduino_comm_port);
			InstallComCallback (arduino_comm_port, LWRS_RXFLAG, 0, '\n', SerialFunc, 0);
			if (x==0 && y==0){
				ServerState(1);
				server_established_flag=1;
				ServerEstablished(1);
			}
			else if (x==0 && y!=0){
				 MessagePopup ("Error", "Make sure your selected Arduino port is correct.");
				 UnregisterTCPServer (server_port);
				 ServerEstablished(0);
			}
			else if (x!=0 && y==0){
				 MessagePopup ("Error", "Make sure your TCP port is correct.");
				 CloseCom (arduino_comm_port);
				 ServerEstablished(0);
			}
			else if (x!=0 && y!=0){
				 MessagePopup ("Error", "Make sure your TCP and Arduino ports are correct.");
				 ServerEstablished(0);
			}
			EnableBreakOnLibraryErrors();
			}
			else
			{
				UnregisterTCPServer (server_port);
				ServerEstablished(0);
				ServerState(0);
				CloseCom (arduino_comm_port);
				arduino_comm_port=3;
				server_established_flag=0;
				a_client_is_inside_flag=0;
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_USERNAME_STR, "");
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_IP_ADDR_STR, "0.0.0.0");
				sprintf(username_sent,"%s",NOT_AUT_USER);
			}
			break;
	}
	return 0;
}

int CVICALLBACK ShowArduinoOutput (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			show_me_what_you_got=!show_me_what_you_got;
			break;
	}
	return 0;
}

int CVICALLBACK ResetArduinoClient (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char msg[50];
			sprintf(msg,"Client requested to reset Arduino.");
			Log(msg);
			PutShowLast(CLIENT,msg);
			SendData(client_con_handle,CLIENT,RESET_ARDUINO,1,"V");
			break;
	}
	return 0;
}

int CVICALLBACK ResetArduinoServer (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char msg[100];
			sprintf(msg,"Initiated Arduino reset.");
			Log(msg);
			PutShowLast(SERVER,msg);
			CloseCom(arduino_comm_port);
			OpenComConfig (arduino_comm_port, "", 9600, 0, 8, 1, 512, 512);
			FlushInQ (arduino_comm_port);
			InstallComCallback (arduino_comm_port, LWRS_RXFLAG, 0, '\n', SerialFunc, 0);
			break;
	}
	return 0;
}

int CVICALLBACK AutoScrollingClient (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle, PANEL_CAUTOSCROLL,&ca_scroll_flag); 
			break;
	}
	return 0;
}

int CVICALLBACK AutoScrollingServer (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle2, PANEL_2_SAUTOSCROLL,&sa_scroll_flag);
			break;
	}
	return 0;
}

int CVICALLBACK ServerClearText (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DeleteTextBoxLines (panelHandle2, PANEL_2_TEXTBOX, 0, -1);
			break;
	}
	return 0;
}

int CVICALLBACK ClientClearText (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DeleteTextBoxLines (panelHandle, PANEL_TEXTBOX_CLIENT, 0, -1);
			break;
	}
	return 0;
}

int CVICALLBACK exit_about (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel (panelHandle4);
			break;
	}
	return 0;
}

int CVICALLBACK about (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int top,left;
			GetPanelAttribute (panelHandle3, ATTR_LEFT, &left);
			GetPanelAttribute (panelHandle3, ATTR_TOP, &top);
			SetPanelAttribute (panelHandle4, ATTR_LEFT, left);
			SetPanelAttribute (panelHandle4, ATTR_TOP, top);
			
			DisplayPanel (panelHandle4);
			break;
	}
	return 0;
}

int CVICALLBACK logo_func (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch(event)
	{
		case EVENT_LEFT_CLICK_UP:
			InetLaunchDefaultWebBrowser ("https://english.afeka.ac.il/");
			break;
	}
	return 0;
}



int CVICALLBACK AuthenticationTimer (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			char msg[200];
			authentication_time_counter++;
			if(authentication_time_counter==20 && a_client_is_inside_flag==0)
			{
				sprintf(msg,"Incompatible client program. You had been kicked out.");
				ServerTCPWrite (a_client_handle, msg, strlen(msg), 0);
				DisconnectTCPClient (a_client_handle);
				a_client_is_inside_flag=0;
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_USERNAME_STR, "");
				SetCtrlVal (panelHandle2, PANEL_2_CLIENT_IP_ADDR_STR, "0.0.0.0");
				SetCtrlAttribute (panelHandle2, PANEL_2_KICK_CLIENT, ATTR_DIMMED, 1);
				ServerState(1);
				sprintf(username_sent,"%s",NOT_AUT_USER);
				authentication_time_counter=0;
				SetCtrlAttribute (panelHandle2, PANEL_2_AUT_TIMER, ATTR_ENABLED, 0);
			}
			else if(a_client_is_inside_flag==1)
			{
				sprintf(msg,"Server cannot accept more connections. You had been kicked out.");
				ServerTCPWrite (go_away_handle, msg, strlen(msg), 0);
				DisconnectTCPClient (go_away_handle);
				authentication_time_counter=0;
				SetCtrlAttribute (panelHandle2, PANEL_2_AUT_TIMER, ATTR_ENABLED, 0);
					
			}
			break;
	}
	return 0;
}


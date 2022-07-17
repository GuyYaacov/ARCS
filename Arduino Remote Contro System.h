/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: exit_client */
#define  PANEL_COMMAND_LINE               2       /* control type: string, callback function: SendCommand */
#define  PANEL_TEXTBOX_CLIENT             3       /* control type: textBox, callback function: (none) */
#define  PANEL_CLIENT_STATUS_LED          4       /* control type: LED, callback function: (none) */
#define  PANEL_FILE_SELECT_BT             5       /* control type: command, callback function: SelectInoFile */
#define  PANEL_SERVER_IP                  6       /* control type: string, callback function: (none) */
#define  PANEL_CONNECTION_STATUS_MSG      7       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG1_4                     8       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG1_3                     9       /* control type: textMsg, callback function: (none) */
#define  PANEL_MSG1                       10      /* control type: textMsg, callback function: (none) */
#define  PANEL_CONNECT_BT                 11      /* control type: command, callback function: ConnectToServer */
#define  PANEL_STRING_PASSWORD            12      /* control type: string, callback function: (none) */
#define  PANEL_STRING_USERNAME            13      /* control type: string, callback function: (none) */
#define  PANEL_BURN_BT                    14      /* control type: command, callback function: SendAndBurn */
#define  PANEL_SEND_BT                    15      /* control type: command, callback function: SendCommand */
#define  PANEL_CLEAR_BT_CLIENT            16      /* control type: command, callback function: ClientClearText */
#define  PANEL_FILE_MSG                   17      /* control type: textMsg, callback function: (none) */
#define  PANEL_BACK_BT                    18      /* control type: command, callback function: BackToMain */
#define  PANEL_PICTURE                    19      /* control type: picture, callback function: (none) */
#define  PANEL_CRESET_ARD_BT              20      /* control type: command, callback function: ResetArduinoClient */
#define  PANEL_TEXTMSG                    21      /* control type: textMsg, callback function: (none) */
#define  PANEL_TCP_PORT                   22      /* control type: numeric, callback function: (none) */
#define  PANEL_MSG1_2                     23      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_4                  24      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_3                  25      /* control type: textMsg, callback function: (none) */
#define  PANEL_CAUTOSCROLL                26      /* control type: radioButton, callback function: AutoScrollingClient */
#define  PANEL_TEXTMSG_2                  27      /* control type: textMsg, callback function: (none) */

#define  PANEL_2                          2       /* callback function: exit_server */
#define  PANEL_2_TEXTBOX                  2       /* control type: textBox, callback function: (none) */
#define  PANEL_2_SRVR_LISTEN_MSG          3       /* control type: textMsg, callback function: (none) */
#define  PANEL_2_SERVER_STATUS_LED        4       /* control type: LED, callback function: (none) */
#define  PANEL_2_CREATE_SERVER            5       /* control type: command, callback function: CreateServer */
#define  PANEL_2_KICK_CLIENT              6       /* control type: command, callback function: KickClient */
#define  PANEL_2_CLEAR_BT_SERVER          7       /* control type: command, callback function: ServerClearText */
#define  PANEL_2_CONNECTION_STATUS_MSG    8       /* control type: textMsg, callback function: (none) */
#define  PANEL_2_CLIENT_IP_ADDR_STR       9       /* control type: string, callback function: (none) */
#define  PANEL_2_CLIENT_USERNAME_STR      10      /* control type: string, callback function: (none) */
#define  PANEL_2_MY_IP_ADDR_STR           11      /* control type: string, callback function: (none) */
#define  PANEL_2_CONNECTED_CLIEN_MSG_4    12      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_CONNECTED_CLIEN_MSG_3    13      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_CONNECTED_CLIEN_MSG_2    14      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_CONNECTED_CLIEN_MSG      15      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_ARDUINO_PORT             16      /* control type: numeric, callback function: (none) */
#define  PANEL_2_TCP_PORT                 17      /* control type: numeric, callback function: (none) */
#define  PANEL_2_BACK_BT                  18      /* control type: command, callback function: BackToMain */
#define  PANEL_2_SRESET_ARD_BT            19      /* control type: command, callback function: ResetArduinoServer */
#define  PANEL_2_TEXTMSG                  20      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_ESTABLISHED_MSG          21      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_TEXTMSG_3                22      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_TEXTMSG_2                23      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_PICTURE                  24      /* control type: picture, callback function: (none) */
#define  PANEL_2_BUADRATE_RING            25      /* control type: ring, callback function: (none) */
#define  PANEL_2_SAUTOSCROLL              26      /* control type: radioButton, callback function: AutoScrollingServer */
#define  PANEL_2_CHECKBOX                 27      /* control type: radioButton, callback function: ShowArduinoOutput */
#define  PANEL_2_AUT_TIMER                28      /* control type: timer, callback function: AuthenticationTimer */

#define  PANEL_3                          3       /* callback function: ExitFromAll */
#define  PANEL_3_CLIENT_BT                2       /* control type: command, callback function: ServerOrClientSelection */
#define  PANEL_3_SERVER_BT                3       /* control type: command, callback function: ServerOrClientSelection */
#define  PANEL_3_MSG_CHOOSE               4       /* control type: textMsg, callback function: (none) */
#define  PANEL_3_About                    5       /* control type: command, callback function: about */

#define  PANEL_4                          4       /* callback function: exit_about */
#define  PANEL_4_TEXTMSG                  2       /* control type: textMsg, callback function: (none) */
#define  PANEL_4_PICTURE                  3       /* control type: picture, callback function: logo_func */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK about(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AuthenticationTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AutoScrollingClient(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AutoScrollingServer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BackToMain(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ClientClearText(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ConnectToServer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CreateServer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exit_about(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exit_client(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exit_server(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ExitFromAll(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK KickClient(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK logo_func(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResetArduinoClient(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ResetArduinoServer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SelectInoFile(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendAndBurn(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SendCommand(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ServerClearText(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ServerOrClientSelection(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ShowArduinoOutput(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif


/************** Static Function Declarations **************/

/************** Global Variable Declarations **************/

/************** Global Function Declarations **************/
void SendData(unsigned int handle, int fromwho, int type, int DataLength, char *data);
int GetData(unsigned int handle, int fromwho, int *type, char *data);
void Log(char *data);
void WhatIsMyIp(char *ip);
int SendFile(unsigned int handle, int fromwho, char *filename, char *filepath, int new_file_type_const);
int GetFile(unsigned int handle, int new_file_type_const);
void SendData(unsigned int handle, int fromwho, int type, int DataLength, char *data);
int GetData(unsigned int handle, int fromwho, int *type, char *data);
int Burn(char *filename, int com);
int Authentication(char username[], char password[]);

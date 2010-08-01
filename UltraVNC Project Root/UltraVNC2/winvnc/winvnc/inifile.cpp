#include "stdhdrs.h"
#include "inifile.h"
//void Set_settings_as_admin(char *mycommand);


IniFile::IniFile()
{
char WORKDIR[MAX_PATH];
	if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
		{
		char* p = strrchr(WORKDIR, '\\');
		if (p == NULL) return;
		*p = '\0';
		}
	strcpy(myInifile,"");
	strcat(myInifile,WORKDIR);//set the directory
	strcat(myInifile,"\\settings\\");
	strcat(myInifile,INIFILE_NAME);
}

void
IniFile::IniFileSetSecure()
{
char WORKDIR[MAX_PATH];
	if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
		{
		char* p = strrchr(WORKDIR, '\\');
		if (p == NULL) return;
		*p = '\0';
		}
	strcpy(myInifile,"");
	strcat(myInifile,WORKDIR);//set the directory
	strcat(myInifile,"\\settings\\");
	strcat(myInifile,INIFILE_NAME);
}

void
IniFile::IniFileSetTemp(char *lpCmdLine)
{	
	strcpy_s(myInifile,260,lpCmdLine);
}

IniFile::~IniFile()
{
}

bool
IniFile::WriteString(char *key1, char *key2,char *value)
{
	//vnclog.Print(LL_INTERR, VNCLOG("%s \n"),myInifile); 
	return (FALSE != WritePrivateProfileString(key1,key2, value,myInifile));
}

bool 
IniFile::WriteInt(char *key1, char *key2,int value)
{
	char       buf[32];
	wsprintf(buf, "%d", value);
	int result=WritePrivateProfileString(key1,key2, buf,myInifile);
	if (result==0) return false;
	return true;
}

int
IniFile::ReadInt(char *key1, char *key2,int Defaultvalue)
{
	return GetPrivateProfileInt(key1, key2, Defaultvalue, myInifile);
}

void 
IniFile::ReadString(char *key1, char *key2,char *value,int valuesize)
{
	GetPrivateProfileString(key1,key2, "",value,valuesize,myInifile);
}

void 
IniFile::ReadPassword(char *value,int valuesize)
{
	GetPrivateProfileStruct("ultravnc","passwd",value,8,myInifile);
}

void //PGM
IniFile::ReadPassword2(char *value,int valuesize) //PGM
{ //PGM
	GetPrivateProfileStruct("ultravnc","passwd2",value,8,myInifile); //PGM
} //PGM

bool
IniFile::WritePassword(char *value)
{
		return (FALSE != WritePrivateProfileStruct("ultravnc","passwd", value,8,myInifile));
}

bool //PGM
IniFile::WritePassword2(char *value) //PGM
{ //PGM
		return (FALSE != WritePrivateProfileStruct("ultravnc","passwd2", value,8,myInifile)); //PGM
} //PGM

bool IniFile::IsWritable()
{
    bool writable = WriteInt("Permissions", "isWritable",1);
    if (writable)
        WritePrivateProfileSection("Permissions", "", myInifile);

    return writable;
}


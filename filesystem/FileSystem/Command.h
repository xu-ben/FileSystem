/*
 * �ļ�����		Command.h 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	�����������е������������ʵ���Լ����������������
 *				����Ŀ¼�������ָ���ļ���ɾ���ļ���ɾ��ָ���ļ���
 *				����ָ��Ŀ¼��ɾ��Ŀ¼��ɾ��ָ��Ŀ¼���ļ���������
 * �޸ļ�¼��	2010/06/04 
 */

#pragma once

#include "Definition.h"
#include "Tools.h"

//�������һ������������������Ͳ���������
void SplitCmdArgs(string &cmd,string &cmdname,string &args)
{
	cmd = Trim(cmd);
	int length = cmd.length();
	int i = cmd.find_first_of(" \t\n\r");
	if(i == -1)
		cmdname = cmd;
	else
	{
		cmdname = cmd.substr(0, i + 1);
		args = cmd.substr(i + 1, length);
	}
	cmdname = Trim(cmdname);
	args = Trim(args);
}

//��ͼ�����������ֳ�������
bool SplitArgs(string &args, string &args1, string &args2)
{
	args = Trim(args);
	int length = args.length();
	int i = args.find_first_of(" \t\n\r");
	if(i == -1)
		return false;

	args1 = args.substr(0, i + 1);
	args2 = args.substr(i + 1, length);
	args1 = Trim(args1);
	args2 = Trim(args2);
	return true;
}

//����������������������һ�������ȷ��Ӧ��ִ��ʲô
bool ResolveCommand(string cmd)
{
	cmd = Trim(cmd);
	const char *chcmd = cmd.c_str();

	int sngcmdnum = 8;
	int index;
	string sngcmds[]={"cd..", "cls", "dir", "diskattrib", "diskdfr", "diskformat", "exit","help"};

	for(index = 0; index < sngcmdnum; index++)
	{
		if(stricmp(chcmd, sngcmds[index].c_str()) == 0)
		{ 
			break;
		}
	}

	switch(index)
	{
	case 0:
		CdDot();
		return true;
	case 1: 
		system("cls");
		return true;
	case 2: 
		ListDir(curdir); 
		return true;
	case 3:
		ShowDiskAttrib();
		return true;
	case 4:
		diskdfr();
		return true;
	case 5:
		DiskFormat();
		return true;
	case 6: 
		return false;
	case 7:
		Help();
		return true;
	default: 
		break;
	}

	string cmdname = string("");
	string args = string("");
	SplitCmdArgs(cmd, cmdname, args);
	const char *chcmdname = cmdname.c_str();
	const char *chcmdargs = args.c_str();


	int cmdnum = 11;
	string cmdnames[]={"cd", "color", "dir", "help", "mk", "mkdir", "revise", "rename", "rm", "rmdir", "type"};


	for(index = 0; index < cmdnum; index++)
	{
		if(stricmp(chcmdname, cmdnames[index].c_str()) == 0)
		{ 
			break;
		}
	}

	char color[20] = {"color "};

	switch(index)
	{
	case 0: 
		Cd(args);
		return true;
	case 1: 
		strcat(color, chcmdargs);
		system(color);
		return true;
	case 2:
		Dir(args);
		return true;
	case 3: 
		Help(args);
		return true;
	case 4:
		Mk(args);
		return true;
	case 5: 
		MkDir(args, curdir);
		return true;
	case 6:
		Revise(args);
		return true;
	case 7:
		Rename(args);
		return true;
	case 8:
		Rm(args);
		return true;
	case 9:
		Rmdir(args);
		return true;
	case 10:
		Type(args);
		return true;
	default: break;
	}

	string args1 = string("");
	string args2 = string("");
	if(SplitArgs(args, args1, args2))
	{
		if(cmdname.compare("cp") == 0)
			Cp(args1, args2);
		else if(cmdname.compare("move") == 0)
			Move(args1, args2);
		else
			cout<<"����Ĳ���������߸�ʽ���ԣ����������롣"<<endl;

		return true;
	}

	cout<<"����Ĳ���������߸�ʽ���ԣ����������롣"<<endl;
	
	return true;

}

//����Ŀ¼
bool Cd(string args)
{
	args = Trim(args);

	short blocknum = -1;
	
	if(args.length() <= 3)
	{
		if(args.compare("A:") == 0)
		{
			curdir = rootdir;
			return true;
		}
		else
		{
			blocknum = FindDirInDir(args, curdir);
		}
	}
	else
	{
		if(args[0] == 'A' && args[1] == ':' && args[2] == '\\')
		{
			blocknum = FindDirInDir(args.substr(3), rootdir);
		}
		else
		{
			blocknum = FindDirInDir(args, curdir);
		}
	}

	if(blocknum < 0)
	{
		cout<<"�ڵ�ǰĿ¼��û���ҵ�Ŀ¼: "<<args<<endl;
		return false;
	}

	Directory dir = getDirectory(blocknum);

	curdir = dir;

	return true;
}

//��ʾ�ļ�����
bool Type(string args)
{
	args = Trim(args);
	
	short stablock = FindFileInDir(args, curdir);
	if(stablock <= 0)
	{
		cout<<"�ڵ�ǰĿ¼��û���ҵ��ļ�: "<<args<<endl;
		return false;
	}

	File f = getFile(stablock);
	string contest = getContestOfFile(f);
	cout<<"********************************************************************************"<<endl;
	cout<<contest<<endl;
	cout<<"********************************************************************************"<<endl;
	return true;
}

//mk(�½��ļ�)�������
bool Mk(string args)
{
	args = Trim(args);
	return NewFile(args, curdir);
}

//mkdir(�½�Ŀ¼)�������
bool MkDir(string args, Directory &dir)
{
	args = Trim(args);

	if(args[0] == 'A' && args[1] == ':' && args[2] == '\\')//�ڸ�Ŀ¼���½�Ŀ¼
	{
		if(!MkDir(args.substr(3), rootdir))
		{
			cout<<"�½�Ŀ¼����!"<<endl;
			return false;
		}
		else
			return true;
	}

	int index = args.find_first_of("\\");
	if(index <= 0)//�ڵ�ǰĿ¼���½�Ŀ¼
		return NewDirectory(args, dir);

	string first = Trim(args.substr(0, index));

	if(!NewDirectory(first, dir))
		return false;

	short fc = FindDirInDir(first, dir);
	if(fc < 0)
	{
		cout<<"�½�Ŀ¼����!"<<endl;
		return false;
	}

	Directory newdir = getDirectory(fc);

	if(!MkDir(args.substr(index + 1), newdir))
		return false;

	return true;
}

//��ʾ��������
bool Help()
{
	const string cmdhelp[] = {"cd          ����ָ����Ŀ¼��",
		"cd..        ������һ��Ŀ¼��",
		"cls         �����Ļ��",
		"color       ����Ĭ�ϵĿ���̨ǰ���ͱ�����ɫ��",
		"cp          ������һ���ļ����Ƶ���һ��λ�á�",		
		"dir         ��ʾһ��Ŀ¼�е��ļ�����Ŀ¼��",	
		"diskattrib  ��ʾ����������Ϣ",
		"diskdfr     ������Ƭ����",
		"exit        �˳�Shell����",	
		"help        �ṩShell����İ�����Ϣ��",
		"mk          ����һ���ļ���",
		"mkdir       ����һ��Ŀ¼��",
		"move        ��һ�������ļ���һ��Ŀ¼�ƶ�����һ��Ŀ¼��",	
		"rename      �޸��ļ�����",
		"rm          ɾ���ļ���",
		"rmdir       ɾ��Ŀ¼��",
		"type        ��ʾ�ļ�����"};
	puts("�й�ĳ���������ϸ��Ϣ������� help ������");
	for(int i = 0; i < 17 ;i++)
		cout<<cmdhelp[i]<<endl;
	cout<<endl;
	return true;
}

//��ʾ����ľ������
bool Help(string args)
{
	int cmdnum = 11, index = 0;
	string cmdnames[]={"cd", "color", "cp", "help", "mk", "mkdir", "move", "rename", "rm", "rmdir", "type"};

	for(index = 0; index < cmdnum; index++)
	{
		if(args.compare(cmdnames[index]) == 0)
		{ 
			break;
		}
	}

	switch(index)
	{
		case 0:    //cd
			cout<<"����ָ����Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    �����ĳ��Ŀ¼ʱ"<<endl;
			cout<<"���á�������    ��ʽ��cd <��Ŀ¼��>"<<endl;
			cout<<"    ʹ��˵����(1)����������cd,������Ѵ��ڵ�Ŀ¼��"<<endl;
			cout<<"              (2)��A�̵ĵ�ǰĿ¼��Ϊ��Ŀ¼   cd A:\\ "<<endl;
			cout<<"                 ��A�̵ĵ�ǰĿ¼��Ϊ��Ŀ¼text��  cd A:\\text "<<endl;
			cout<<"���������ӣ�    A:\\>cd system���ڵ�ǰ������A���´�����Ŀ¼system��"<<endl;
			break;
		case 1:    //color
			cout<<"����Ĭ�ϵĿ���̨ǰ���ͱ�����ɫ��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��ı�����ͱ���ɫʱ"<<endl;
			cout<<"���á�������    ��ʽ��color <attr>"<<endl;
			cout<<"    ʹ��˵����  attr     ָ������̨�������ɫ����"<<endl;
			cout<<"           ��ɫ����������ʮ����������ָ�� -- ��һ��Ϊ�������ڶ�����Ϊ"<<endl;
			cout<<"       ǰ����ÿ�����ֿ���Ϊ�����κ�ֵ֮һ��"<<endl; 
			cout<<"          0 = ��ɫ       8 = ��ɫ"<<endl;
			cout<<"          1 = ��ɫ       9 = ����ɫ"<<endl;
			cout<<"          2 = ��ɫ       A = ����ɫ"<<endl;
			cout<<"          3 = ǳ��ɫ     B = ��ǳ��ɫ"<<endl;
			cout<<"          4 = ��ɫ       C = ����ɫ"<<endl;
			cout<<"          5 = ��ɫ       D = ����ɫ"<<endl;
			cout<<"          6 = ��ɫ       E = ����ɫ"<<endl;
			cout<<"          7 = ��ɫ       F = ����ɫ"<<endl;
			cout<<"���������ӣ�    A:\\>color 6D��������̨����ɫ��Ϊ��ɫ��ǰ��ɫ��Ϊ����ɫ��"<<endl;
			break;	
		case 2:     //cp
			cout<<"������һ���ļ����Ƶ���һ��λ�á�"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    �뽫�ļ����Ƶ�����λ��ʱ"<<endl;
			cout<<"���á�������    ��ʽ��COPY [·��]<Դ�ļ���> [·��][Ŀ���ļ���]"<<endl;
			cout<<"���������ӣ�    copy  A:\\hello.txt d:\\hello.txt    ��c���Ƶ�d"<<endl;
			
			break;
		case 3:      //help
			cout<<"�ṩShell����İ�����Ϣ��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��õ����ڳ���DOS����İ���ʱ"<<endl;
			cout<<"���á�������	   HELP [command]    command - ��ʾ������İ�����Ϣ��"<<endl;
			cout<<"���������ӣ�    help dir"<<endl;
			break;
		case 4:     //mk
			cout<<"����һ���ļ���"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    �봴���µ��ļ�ʱ"<<endl;
			cout<<"���á�������    ��ʽ��MK [�̷���][·����]<�ļ���>"<<endl;
			cout<<" ʹ��˵��:��1�����̷�����ָ��Ҫ�����ļ��Ĵ�����������ĸ����ʡ����Ϊ��ǰ������"<<endl;
			cout<<"          ��2����·��������Ҫ�������ļ����ϼ�Ŀ¼������ȱʡ���ڵ�ǰĿ¼�¡�"<<endl;
			cout<<"���������ӣ�    A:\\>MK system\\abc.txt ����system ��Ŀ¼�´���abc.txt�ļ���"<<endl;            
			break;
		case 5:    //mkdir
			cout<<"����һ��Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    �봴���µ�Ŀ¼ʱ"<<endl;
			cout<<"���á�������    ��ʽ��MKDIR[�̷���][·����]<��Ŀ¼��>"<<endl;
			cout<<" ʹ��˵������1�����̷�����ָ��Ҫ������Ŀ¼����������ĸ����ʡ����Ϊ��ǰ��������"<<endl;
			cout<<"           ��2����·��������Ҫ��������Ŀ¼���ϼ�Ŀ¼������ȱʡ���ڵ�ǰĿ¼�¡�"<<endl;
			cout<<"���������ӣ�    A:\\>MD system ���ڵ�ǰ������A���´�����Ŀ¼system��"<<endl;
			cout<<"                A:\\>MD system\\text ����system ��Ŀ¼���ٴ���text��Ŀ¼��"<<endl;
			break;
		case 6:  //move
			cout<<"��һ�������ļ���һ��Ŀ¼�ƶ�����һ��Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��Ҫ�ƶ��ļ���λ��ʱ"<<endl;
			cout<<"���á�������    Ҫ�ƶ�����һ���ļ�:"<<endl;
			cout<<"                [drive:][path]dirname1  ָ��Ҫ��������Ŀ¼��"<<endl;
			cout<<"���������ӣ�    move e:\\f f:\\f "<<endl;
			break;
		case 7:    //rename
			cout<<"�޸��ļ�����"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��Ҫ�ı��ļ�����ʱ"<<endl;
			cout<<"���á�������    ��ʽ��rename [�̷�:][·��]<���ļ���> "<<endl;
			cout<<"     ʹ��˵���� ����������ļ����󣬻������ʾ�����µ��ļ���"<<endl;
			cout<<"            ��ʱ�������ļ�����ϵͳ�Զ���Դ�ļ������ġ�"<<endl;
			cout<<"���������ӣ�    ���ļ�a.txt����Ϊb.txt:"<<endl;
			cout<<"                 A:\\>rename a.txt"<<endl;
			cout<<"                 A:\\>b.txt"<<endl;
			break;
		case 8:    //rm
			cout<<"ɾ���ļ���"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��ɾ������Ҫ���ļ�ʱ"<<endl;
			cout<<"���á�������    ��ʽ��RM[�̷���][·����][�ļ���]"<<endl;
			cout<<"     ʹ��˵���� ָ��һ���������ļ���ͨ����ɱ�����ɾ������ļ���"<<endl;
			cout<<"���������ӣ�    ɾ��A��textĿ¼�µ�abc.txt �ļ��� A:\\>RM A:\\text\\abc.txt ";
			break;
		case 9:    //rmdir
			cout<<"ɾ��Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��Ҫɾ����Ҫ��Ŀ¼ʱ"<<endl;
			cout<<"���á�������    ��ʽ��RMDIR [�̷���][·����][Ŀ¼��]"<<endl;
			cout<<"���������ӣ�    ��A�̸�Ŀ¼����text��Ŀ¼����text��Ŀ¼����abc.txt�ļ���"<<endl;
			cout<<"            ɾ��textĿ¼�ķ����ǣ� A:\\>rm text\\*.* "<<endl;
			cout<<"                                   A:\\>rmdir text "<<endl;
			break;
		case 10:   //type
			cout<<"��ʾ�ļ����ݡ�"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��Ҫ��ʾ�ļ�������ʱ"<<endl;
			cout<<"���á�������    ��ʽ��type �ļ���.txt"<<endl;
			cout<<"���������ӣ�    A:\\>type abc.txt"<<endl;
			break;
	}
	int sngcmdnum = 7;

	string sngcmds[]={"cd..", "cls", "dir", "diskattrib", "diskdfr", "exit","help"};

	for(index = 0; index < sngcmdnum; index++)
	{
		if(args.compare(sngcmds[index]) == 0)
		{ 
			break;
		}
	}
	switch(index)
	{
		case 0:   //cd..
			cout<<"������һ��Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    �뷵����һ��Ŀ¼ʱ"<<endl;
			cout<<"���á�������    ��ʽ��CD.."<<endl;
			cout<<"                cd ..\\system �ȷ��ظ�Ŀ¼,�ٽ��븸Ŀ¼�µ�98��Ŀ¼"<<endl;
			cout<<"���������ӣ�    A:\\>cd.."<<endl;
			break;
		case 1:    //cls
			cout<<"�����Ļ��"<<endl;
			break;
		case 2:    //dir
			cout<<"��ʾһ��Ŀ¼�е��ļ�����Ŀ¼��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��鿴Ŀ¼�е�����ʱ"<<endl;
			cout<<"���á�������    ��ʽ:DIR [�̷�:][·��][�ļ���] "<<endl;
			cout<<"���������ӣ�    A:\\>dir system"<<endl;
			break;
		case 3:   //diskattrib
			cout<<"��ʾ���̵�������Ϣ��"<<endl;
			cout<<endl;
			cout<<"���á�������    A:\\>diskattrib"<<endl;
			break;
		case 4:   //diskdfr
			cout<<"������Ƭ����."<<endl;
			cout<<endl;
			cout<<"���á�������    A:\\>diskdfr"<<endl;
			break;
		case 5:    //exit
			cout<<"�˳�Shell����"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ���˳�����ʱ"<<endl;
			cout<<"���á�������    EXIT "<<endl;
			cout<<"���������ӣ�    C:\\>exit"<<endl;
			break;
		case 6:    //help
			cout<<"�ṩShell����İ�����Ϣ��"<<endl;
			cout<<endl;
			cout<<"�����ó��ϣ�    ��õ����ڳ���DOS����İ���ʱ"<<endl;
			cout<<"���á�������	   HELP [command]     command - ��ʾ������İ�����Ϣ��"<<endl;
			cout<<"���������ӣ�    help dir"<<endl;
			break;
	}
	return true;
}

//�жϵ�ǰĿ¼�Ƿ��Ǹ�Ŀ¼�����Ǹ�Ŀ¼���򷵻�false�������ٷ�����һ��Ŀ¼�����򷵻�true
bool CdDot()
{
	if(!UpDir(curdir))
	{
		cout<<"��ǰĿ¼�Ѿ��Ǹ�Ŀ¼�������ٷ����ϼ�Ŀ¼"<<endl;
		return false;
	}
	return true;
}

bool Cp(string args1, string args2)
{
	args1 = Trim(args1);
	args2 = Trim(args2);

	short sourceindex = FindFileInAbsPath(args1);

	if(sourceindex < 0)
	{
		cout<<"�Ҳ����ļ� "<<args1<<endl;
		return false;
	}

	short aimdirindex = FindDirInAbsPath(args2);

	if(aimdirindex < 0)
	{
		cout<<"�Ҳ���Ŀ��Ŀ¼ "<<args2<<endl;
		return false;
	}

	File fs = getFile(sourceindex);
	Directory dir = getDirectory(aimdirindex);

	if(!NewFile(fs, dir))
	{
		cout<<"�����ļ�ʧ��!"<<endl;
		return false;
	}

	return true;
}

//�ڸ�����Ŀ¼��,����ָ���ļ���ɾ���ļ�
bool Rm(string args, Directory dir)
{
	args = Trim(args);

	short stablock = FindFileInDir(args, dir);
	if(stablock <= 0)
	{
		cout<<"��Ŀ¼ "<<dir.dirname<<" ��û���ҵ��ļ�: "<<args<<endl;
		return false;
	}

	File file = getFile(stablock);
	if(!Rm(file))
		return false;

	int i;	
	for(i = 0; i < dir.filenum; i++)
	{
		if(dir.chifiles[i] == stablock)
			break;
	}

	if(i == dir.filenum)
		return false;

	for(int j = i + 1; j < dir.filenum; j++)
	{
		dir.chifiles[j-1] = dir.chifiles[j];
	}

	dir.filenum--;

	UpdateDirectory(dir);

	if(dircmp(&dir, &curdir))
	{
		curdir = dir;
	}

	return true;
}

//����ָ���ļ���(�����Ǿ���·��)ɾ���ļ�
bool Rm(string args)
{
	args = Trim(args);

	int index = args.find_last_of("\\");

	if(index < 0)
	{
		return Rm(args, curdir);
	}

	string dirpath = args.substr(0, index);
	short dirindex = FindDirInAbsPath(dirpath);
	if(dirindex < 0)
	{
		cout<<"�Ҳ���Ŀ¼ "<<Trim(dirpath)<<endl;
		return false;
	}

	Directory dir = getDirectory(dirindex);

	string newargs = args.substr(index + 1);

	return Rm(newargs, dir);
}

//ɾ��ָ���ļ�
bool Rm(File file)
{
	return FreeSpace(file.stablock, file.blocknum);
}

//dir(��ʾĿ¼��Ϣ)�������
bool Dir(string dirname)
{
	dirname = Trim(dirname);

	short blocknum = -1;
	
	if(dirname.length() <= 3)
	{
		if(dirname.compare("A:") == 0)
		{
			ListDir(rootdir);
			return true;
		}
		else
		{
			blocknum = FindDirInDir(dirname, curdir);
		}
	}
	else
	{
		if(dirname[0] == 'A' && dirname[1] == ':' && dirname[2] == '\\')
		{
			blocknum = FindDirInDir(dirname.substr(3), rootdir);
		}
		else
		{
			blocknum = FindDirInDir(dirname, curdir);
		}
	}

	if(blocknum < 0)
		return false;

	Directory dir = getDirectory(blocknum);

	ListDir(dir);

	return true;
}

bool Md(string args)
{
	return true;
}

bool Move(string args1, string args2)
{
	args1 = Trim(args1);
	args2 = Trim(args2);

	if(!Cp(args1, args2))
		return false;
	
	if(!Rm(args1))
		return false;

	return true;
}

//����ָ��Ŀ¼��ɾ��Ŀ¼
bool Rmdir(string args)
{
	args = Trim(args);

	short stablock = FindDirInDir(args, curdir);

	if(stablock <= 0)
	{
		cout<<"�ڵ�ǰĿ¼��û���ҵ�Ŀ¼: "<<args<<endl;
		return false;
	}

	Directory dir = getDirectory(stablock);

	if(!Rmdir(dir))
	{
		cout<<"ɾ��Ŀ¼ "<<args<<" ʧ�ܣ�"<<endl;
		return false;
	}

	int i;
	for(i = 0; i < curdir.childdirnum; i++)
	{
		if(curdir.chidirs[i] == stablock)
			break;
	}

	if(i == curdir.childdirnum)
		return false;

	for(int j = i + 1; j < curdir.childdirnum; j++)
	{
		curdir.chidirs[j-1] = curdir.chidirs[j];
	}

	curdir.childdirnum--;

	UpdateDirectory(curdir);
	
	return true;
}

//ɾ��ָ��Ŀ¼
bool Rmdir(Directory dir)
{
	bool flag = true;//������
	int i;

	for(i = 0; i < dir.childdirnum; i++)
	{
		flag = Rmdir(getDirectory(dir.chidirs[i]));
		if(!flag)
			return false;
	}

	for(i = 0; i < dir.filenum; i++)
	{
		flag = Rm(getFile(dir.chifiles[i]));
		if(!flag)
			return false;
	}

	return FreeSpace(dir.ownindex, 1);
}

//�޸��ļ�����
bool Revise(string args)
{
	args = Trim(args);
	
	short stablock = FindFileInDir(args, curdir);
	if(stablock <= 0)
	{
		cout<<"�ڵ�ǰĿ¼��û���ҵ��ļ�: "<<args<<endl;
		return false;
	}

	File f = getFile(stablock);

	cout<<"�����������ݵ�����:"<<endl;
	int rownum;
	cin>>rownum;
	cin.get();

	cout<<"�������ļ� "<<f.filename<<" ��������:"<<endl;
	char row[CHAR_NUM_IN_A_ROW];
	string contest = string("");
	const char *filecontest = contest.c_str();

	for(int i = 0; i < rownum; i++)
	{
		cin.getline(row, CHAR_NUM_IN_A_ROW);
		contest += row;
		contest += "\n";
	}

	cout<<"��������ݳɹ�����ǰ�ļ� "<<f.filename<<" �е�����Ϊ:"<<endl;
	cout<<"********************************************************************************"<<endl;
	cout<<contest<<endl;
	cout<<"********************************************************************************"<<endl;
	filecontest = contest.c_str();
	
	if(!SaveFile(f, filecontest, false))
		return false;

	//���ļ�����Ŀ¼��
	for(int j = 0; j < curdir.filenum; j++)
	{
		if(curdir.chifiles[j] == stablock)
		{
			curdir.chifiles[j] = f.stablock;
		}
	}

	UpdateDirectory(curdir);//���¸�Ŀ¼

	return true;
}

//�ļ���������
bool Rename(string args)
{
	args = Trim(args);

	short stablock = FindDirInDir(args, curdir);
	if(stablock > 0)
	{
		Directory dir = getDirectory(stablock);
		cout<<"����������Ŀ¼: "<<dir.dirname<<" ������:"<<endl;
		cin>>dir.dirname;
		cin.ignore();
		UpdateDirectory(dir);
		return true;
	}

	stablock = FindFileInDir(args, curdir);
	if(stablock > 0)
	{
		File file = getFile(stablock);
		cout<<"�����������ļ�: "<<file.filename<<" ������:"<<endl;
		cin>>file.filename;
		cin.ignore();
		UpdateFile(file);
		return true;
	}

	cout<<"�ڵ�ǰĿ¼��û���ҵ��ļ���Ŀ¼: "<<args<<endl;

	return false;
}

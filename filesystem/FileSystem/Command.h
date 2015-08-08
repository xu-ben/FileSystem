/*
 * 文件名：		Command.h 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	包含本程序中的所有命令函数的实现以及命令解析函数，如
 *				进入目录命令、根据指定文件名删除文件、删除指定文件、
 *				根据指定目录名删除目录、删除指定目录、文件重新名。
 * 修改记录：	2010/06/04 
 */

#pragma once

#include "Definition.h"
#include "Tools.h"

//将输入的一串命令解析成命令名和参数两部分
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

//试图将命令参数拆分成两部分
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

//命令解析函数，解析输入的一串命令，以确定应该执行什么
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
			cout<<"输入的不是命令，或者格式不对，请重新输入。"<<endl;

		return true;
	}

	cout<<"输入的不是命令，或者格式不对，请重新输入。"<<endl;
	
	return true;

}

//进入目录
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
		cout<<"在当前目录中没有找到目录: "<<args<<endl;
		return false;
	}

	Directory dir = getDirectory(blocknum);

	curdir = dir;

	return true;
}

//显示文件内容
bool Type(string args)
{
	args = Trim(args);
	
	short stablock = FindFileInDir(args, curdir);
	if(stablock <= 0)
	{
		cout<<"在当前目录中没有找到文件: "<<args<<endl;
		return false;
	}

	File f = getFile(stablock);
	string contest = getContestOfFile(f);
	cout<<"********************************************************************************"<<endl;
	cout<<contest<<endl;
	cout<<"********************************************************************************"<<endl;
	return true;
}

//mk(新建文件)命令处理函数
bool Mk(string args)
{
	args = Trim(args);
	return NewFile(args, curdir);
}

//mkdir(新建目录)命令处理函数
bool MkDir(string args, Directory &dir)
{
	args = Trim(args);

	if(args[0] == 'A' && args[1] == ':' && args[2] == '\\')//在根目录下新建目录
	{
		if(!MkDir(args.substr(3), rootdir))
		{
			cout<<"新建目录出错!"<<endl;
			return false;
		}
		else
			return true;
	}

	int index = args.find_first_of("\\");
	if(index <= 0)//在当前目录下新建目录
		return NewDirectory(args, dir);

	string first = Trim(args.substr(0, index));

	if(!NewDirectory(first, dir))
		return false;

	short fc = FindDirInDir(first, dir);
	if(fc < 0)
	{
		cout<<"新建目录出错!"<<endl;
		return false;
	}

	Directory newdir = getDirectory(fc);

	if(!MkDir(args.substr(index + 1), newdir))
		return false;

	return true;
}

//显示所有命令
bool Help()
{
	const string cmdhelp[] = {"cd          进入指定的目录。",
		"cd..        返回上一级目录。",
		"cls         清除屏幕。",
		"color       设置默认的控制台前景和背景颜色。",
		"cp          将至少一个文件复制到另一个位置。",		
		"dir         显示一个目录中的文件和子目录。",	
		"diskattrib  显示磁盘物理信息",
		"diskdfr     磁盘碎片整理",
		"exit        退出Shell程序。",	
		"help        提供Shell命令的帮助信息。",
		"mk          创建一个文件。",
		"mkdir       创建一个目录。",
		"move        将一个或多个文件从一个目录移动到另一个目录。",	
		"rename      修改文件名。",
		"rm          删除文件。",
		"rmdir       删除目录。",
		"type        显示文件内容"};
	puts("有关某个命令的详细信息，请键入 help 命令名");
	for(int i = 0; i < 17 ;i++)
		cout<<cmdhelp[i]<<endl;
	cout<<endl;
	return true;
}

//显示命令的具体解析
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
			cout<<"进入指定的目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想进入某个目录时"<<endl;
			cout<<"［用　　法］    格式：cd <子目录名>"<<endl;
			cout<<"    使用说明：(1)不能是输入cd,必须加已存在的目录名"<<endl;
			cout<<"              (2)将A盘的当前目录改为根目录   cd A:\\ "<<endl;
			cout<<"                 将A盘的当前目录改为子目录text下  cd A:\\text "<<endl;
			cout<<"［例　　子］    A:\\>cd system（在当前驱动器A盘下创建子目录system）"<<endl;
			break;
		case 1:    //color
			cout<<"设置默认的控制台前景和背景颜色。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想改变字体和背景色时"<<endl;
			cout<<"［用　　法］    格式：color <attr>"<<endl;
			cout<<"    使用说明：  attr     指定控制台输出的颜色属性"<<endl;
			cout<<"           颜色属性由两个十六进制数字指定 -- 第一个为背景，第二个则为"<<endl;
			cout<<"       前景。每个数字可以为以下任何值之一："<<endl; 
			cout<<"          0 = 黑色       8 = 灰色"<<endl;
			cout<<"          1 = 蓝色       9 = 淡蓝色"<<endl;
			cout<<"          2 = 绿色       A = 淡绿色"<<endl;
			cout<<"          3 = 浅绿色     B = 淡浅绿色"<<endl;
			cout<<"          4 = 红色       C = 淡红色"<<endl;
			cout<<"          5 = 紫色       D = 淡紫色"<<endl;
			cout<<"          6 = 黄色       E = 淡黄色"<<endl;
			cout<<"          7 = 白色       F = 亮白色"<<endl;
			cout<<"［例　　子］    A:\\>color 6D（将控制台背景色改为黄色，前景色改为淡紫色）"<<endl;
			break;	
		case 2:     //cp
			cout<<"将至少一个文件复制到另一个位置。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想将文件复制到其他位置时"<<endl;
			cout<<"［用　　法］    格式：COPY [路径]<源文件名> [路径][目标文件名]"<<endl;
			cout<<"［例　　子］    copy  A:\\hello.txt d:\\hello.txt    从c复制到d"<<endl;
			
			break;
		case 3:      //help
			cout<<"提供Shell命令的帮助信息。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想得到关于程序DOS命令的帮助时"<<endl;
			cout<<"［用　　法］	   HELP [command]    command - 显示该命令的帮助信息。"<<endl;
			cout<<"［例　　子］    help dir"<<endl;
			break;
		case 4:     //mk
			cout<<"创建一个文件。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想创建新的文件时"<<endl;
			cout<<"［用　　法］    格式：MK [盘符：][路径名]<文件名>"<<endl;
			cout<<" 使用说明:（1）“盘符”：指定要建立文件的磁盘驱动器字母，若省略则为当前驱动器"<<endl;
			cout<<"          （2）“路径名”：要建立的文件的上级目录名，若缺省则建在当前目录下。"<<endl;
			cout<<"［例　　子］    A:\\>MK system\\abc.txt （在system 子目录下创建abc.txt文件）"<<endl;            
			break;
		case 5:    //mkdir
			cout<<"创建一个目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想创建新的目录时"<<endl;
			cout<<"［用　　法］    格式：MKDIR[盘符：][路径名]<子目录名>"<<endl;
			cout<<" 使用说明：（1）“盘符”：指定要建立子目录的驱动器字母，若省略则为当前驱动器；"<<endl;
			cout<<"           （2）“路径名”：要建立的子目录的上级目录名，若缺省则建在当前目录下。"<<endl;
			cout<<"［例　　子］    A:\\>MD system （在当前驱动器A盘下创建子目录system）"<<endl;
			cout<<"                A:\\>MD system\\text （在system 子目录下再创建text子目录）"<<endl;
			break;
		case 6:  //move
			cout<<"将一个或多个文件从一个目录移动到另一个目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想要移动文件的位置时"<<endl;
			cout<<"［用　　法］    要移动至少一个文件:"<<endl;
			cout<<"                [drive:][path]dirname1  指定要重命名的目录。"<<endl;
			cout<<"［例　　子］    move e:\\f f:\\f "<<endl;
			break;
		case 7:    //rename
			cout<<"修改文件名。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想要改变文件名称时"<<endl;
			cout<<"［用　　法］    格式：rename [盘符:][路径]<旧文件名> "<<endl;
			cout<<"     使用说明： 当输入完旧文件名后，会出现提示输入新的文件名"<<endl;
			cout<<"            此时输入新文件名后，系统自动将源文件名更改。"<<endl;
			cout<<"［例　　子］    把文件a.txt改名为b.txt:"<<endl;
			cout<<"                 A:\\>rename a.txt"<<endl;
			cout<<"                 A:\\>b.txt"<<endl;
			break;
		case 8:    //rm
			cout<<"删除文件。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想删除不需要的文件时"<<endl;
			cout<<"［用　　法］    格式：RM[盘符：][路径名][文件名]"<<endl;
			cout<<"     使用说明： 指定一个或数个文件。通配符可被用来删除多个文件。"<<endl;
			cout<<"［例　　子］    删除A盘text目录下的abc.txt 文件： A:\\>RM A:\\text\\abc.txt ";
			break;
		case 9:    //rmdir
			cout<<"删除目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想要删除不要的目录时"<<endl;
			cout<<"［用　　法］    格式：RMDIR [盘符：][路径名][目录名]"<<endl;
			cout<<"［例　　子］    在A盘根目录下有text子目录，在text子目录中有abc.txt文件，"<<endl;
			cout<<"            删除text目录的方法是： A:\\>rm text\\*.* "<<endl;
			cout<<"                                   A:\\>rmdir text "<<endl;
			break;
		case 10:   //type
			cout<<"显示文件内容。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想要显示文件的内容时"<<endl;
			cout<<"［用　　法］    格式：type 文件名.txt"<<endl;
			cout<<"［例　　子］    A:\\>type abc.txt"<<endl;
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
			cout<<"返回上一级目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想返回上一级目录时"<<endl;
			cout<<"［用　　法］    格式：CD.."<<endl;
			cout<<"                cd ..\\system 先返回父目录,再进入父目录下的98子目录"<<endl;
			cout<<"［例　　子］    A:\\>cd.."<<endl;
			break;
		case 1:    //cls
			cout<<"清除屏幕。"<<endl;
			break;
		case 2:    //dir
			cout<<"显示一个目录中的文件和子目录。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想查看目录中的内容时"<<endl;
			cout<<"［用　　法］    格式:DIR [盘符:][路径][文件名] "<<endl;
			cout<<"［例　　子］    A:\\>dir system"<<endl;
			break;
		case 3:   //diskattrib
			cout<<"显示磁盘的物理信息。"<<endl;
			cout<<endl;
			cout<<"［用　　法］    A:\\>diskattrib"<<endl;
			break;
		case 4:   //diskdfr
			cout<<"磁盘碎片整理."<<endl;
			cout<<endl;
			cout<<"［用　　法］    A:\\>diskdfr"<<endl;
			break;
		case 5:    //exit
			cout<<"退出Shell程序。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想退出程序时"<<endl;
			cout<<"［用　　法］    EXIT "<<endl;
			cout<<"［例　　子］    C:\\>exit"<<endl;
			break;
		case 6:    //help
			cout<<"提供Shell命令的帮助信息。"<<endl;
			cout<<endl;
			cout<<"［适用场合］    想得到关于程序DOS命令的帮助时"<<endl;
			cout<<"［用　　法］	   HELP [command]     command - 显示该命令的帮助信息。"<<endl;
			cout<<"［例　　子］    help dir"<<endl;
			break;
	}
	return true;
}

//判断当前目录是否是根目录，若是根目录，则返回false，不能再返回上一级目录；否则返回true
bool CdDot()
{
	if(!UpDir(curdir))
	{
		cout<<"当前目录已经是根目录，不能再返回上级目录"<<endl;
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
		cout<<"找不到文件 "<<args1<<endl;
		return false;
	}

	short aimdirindex = FindDirInAbsPath(args2);

	if(aimdirindex < 0)
	{
		cout<<"找不到目标目录 "<<args2<<endl;
		return false;
	}

	File fs = getFile(sourceindex);
	Directory dir = getDirectory(aimdirindex);

	if(!NewFile(fs, dir))
	{
		cout<<"复制文件失败!"<<endl;
		return false;
	}

	return true;
}

//在给定的目录下,根据指定文件名删除文件
bool Rm(string args, Directory dir)
{
	args = Trim(args);

	short stablock = FindFileInDir(args, dir);
	if(stablock <= 0)
	{
		cout<<"在目录 "<<dir.dirname<<" 中没有找到文件: "<<args<<endl;
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

//根据指定文件名(可以是绝对路径)删除文件
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
		cout<<"找不到目录 "<<Trim(dirpath)<<endl;
		return false;
	}

	Directory dir = getDirectory(dirindex);

	string newargs = args.substr(index + 1);

	return Rm(newargs, dir);
}

//删除指定文件
bool Rm(File file)
{
	return FreeSpace(file.stablock, file.blocknum);
}

//dir(显示目录信息)命令处理函数
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

//根据指定目录名删除目录
bool Rmdir(string args)
{
	args = Trim(args);

	short stablock = FindDirInDir(args, curdir);

	if(stablock <= 0)
	{
		cout<<"在当前目录中没有找到目录: "<<args<<endl;
		return false;
	}

	Directory dir = getDirectory(stablock);

	if(!Rmdir(dir))
	{
		cout<<"删除目录 "<<args<<" 失败！"<<endl;
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

//删除指定目录
bool Rmdir(Directory dir)
{
	bool flag = true;//出错标记
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

//修改文件内容
bool Revise(string args)
{
	args = Trim(args);
	
	short stablock = FindFileInDir(args, curdir);
	if(stablock <= 0)
	{
		cout<<"在当前目录中没有找到文件: "<<args<<endl;
		return false;
	}

	File f = getFile(stablock);

	cout<<"请输入新内容的行数:"<<endl;
	int rownum;
	cin>>rownum;
	cin.get();

	cout<<"请输入文件 "<<f.filename<<" 的新内容:"<<endl;
	char row[CHAR_NUM_IN_A_ROW];
	string contest = string("");
	const char *filecontest = contest.c_str();

	for(int i = 0; i < rownum; i++)
	{
		cin.getline(row, CHAR_NUM_IN_A_ROW);
		contest += row;
		contest += "\n";
	}

	cout<<"添加新内容成功，当前文件 "<<f.filename<<" 中的内容为:"<<endl;
	cout<<"********************************************************************************"<<endl;
	cout<<contest<<endl;
	cout<<"********************************************************************************"<<endl;
	filecontest = contest.c_str();
	
	if(!SaveFile(f, filecontest, false))
		return false;

	//将文件更新目录中
	for(int j = 0; j < curdir.filenum; j++)
	{
		if(curdir.chifiles[j] == stablock)
		{
			curdir.chifiles[j] = f.stablock;
		}
	}

	UpdateDirectory(curdir);//更新父目录

	return true;
}

//文件重新命名
bool Rename(string args)
{
	args = Trim(args);

	short stablock = FindDirInDir(args, curdir);
	if(stablock > 0)
	{
		Directory dir = getDirectory(stablock);
		cout<<"请重新输入目录: "<<dir.dirname<<" 的名称:"<<endl;
		cin>>dir.dirname;
		cin.ignore();
		UpdateDirectory(dir);
		return true;
	}

	stablock = FindFileInDir(args, curdir);
	if(stablock > 0)
	{
		File file = getFile(stablock);
		cout<<"请重新输入文件: "<<file.filename<<" 的名称:"<<endl;
		cin>>file.filename;
		cin.ignore();
		UpdateFile(file);
		return true;
	}

	cout<<"在当前目录中没有找到文件或目录: "<<args<<endl;

	return false;
}

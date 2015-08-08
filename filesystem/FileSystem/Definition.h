/*
 * 文件名：		Definition.h 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	此头文件中包含了本程序所需的头文件，进行了宏定义，
 *				结构定义和变量定义以及对头文件PhysicFile.h、LogicFile.h、
 *				Command.h、DirectorySearch.h中定义的函数进行函数声明
 * 修改记录：	2010/06/04 
 */

#pragma once

//////////////////////////////////////////////////////////////////
//本程序需要包含的头文件

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <windows.h>


using namespace std;


///////////////////////////////////////////////////////////////////
//宏定义
#define SYSTEM_SIZE	30720			//文件系统大小，30K字节
#define SYSTEM_FILE_NAME	"A"		//实际存储本文件系统的文件名称

#ifndef OVERFLOW
#define OVERFLOW -2
#endif


#define CHAR_NUM_IN_A_ROW 250
#define BLOCK_SIZE	128				//块大小(单位:字节)
#define BLOCK_NUM	240				//块数量
#define DIR_NAME_LENGTH 24			//目录名长度
#define FILE_NAME_LENGTH 40			//普通文件名长度
#define CHILD_DIR_NUM	12			//每个目录下直接子目录的最大数量
#define FILE_NUM	16				//每个目录下直接存放的文件的最大数量

//////////////////////////////////////////////////////////////////
//结构定义和变量定义

//文件(目录)类型，dir表示目录，其余四种分别表示四种类型的文件
enum FileStyle		
{
	DIR,
	TXT,
	CPP,
	C,
	H
};

//引导块
struct BootBlock	
{
	char systeminfo[BLOCK_SIZE];
};

//超级块
struct SuperBlock	
{
	int blocknum;
	int blocksize;
	char authorinfo[BLOCK_SIZE - sizeof(int) * 2];
};

//空闲连续块
struct IdleBlock	
{
	short startblock;	//空闲连续块起始块号
	short endblock;		//空闲连续块终止（包含此块）块号
};

//空闲空间管理表
struct IdlManTbl
{
	short idlblocknum;//空闲块个数
	IdleBlock blocks[(BLOCK_SIZE * 4 - sizeof(short)) / sizeof(short) / 2];//空闲块数组
};

//定义目录结构体
struct Directory
{
	FileStyle filestyle;				//文件(目录)类型
	short fatherindex;					//父目录存放的块号
	short ownindex;						//本目录存放的块号
	short childdirnum;					//当前目录的子目录数量
	short chidirs[CHILD_DIR_NUM];		//当前目录的子目录列表
	short filenum;						//当前目录中文件数量
	short chifiles[FILE_NUM];			//当前目录中的文件列表
	tm create_time;						//目录创建时间
	char dirname[DIR_NAME_LENGTH];		//目录名
};

//定义文件结构
struct File
{
	FileStyle filestyle;				//文件(目录)类型
	short fatherindex;					//父目录存放的块号
	int   filesize;						//文件大小（单位：字节）
	short stablock;						//文件起始块号
	short blocknum;						//文件当前占用磁盘块数
	tm create_time;						//文件创建时间
	tm last_update_time;				//上次修改时间
	char filename[FILE_NAME_LENGTH];	//文件名
};

//文件和目录联合体，将文件和目录当作同样的结点加到文件目录结构中去
union FileNode	
{
	File file;
	Directory dir;
};


fstream FileSystem;			//定义文件流

IdlManTbl imt;				//空闲块管理表

BootBlock bootblock;		//定义引导块变量

SuperBlock superblock;      //定义超级快变量

Directory rootdir;			//根目录

Directory curdir;			//当前目录

//////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////
//函数声明


//////////////////////////////////////////////////////////////////
//PhysicFile.h

//分配磁盘空间
short AllocSpace(short blocknum);

//关闭文件系统
bool CloseFileSystem();

//磁盘碎片整理
void diskdfr();

//释放磁盘空间
bool FreeSpace(short stablock, short blocknum);

//根据块号从磁盘中取出一个目录
Directory getDirectory(short blockindex);

//根据文件结构从虚拟磁盘取出文件内容
string getContestOfFile(File f);

//根据块号从磁盘中取出一个文件
File getFile(short blockindex);

//安装操作系统
void installos();

//磁盘格式化
bool DiskFormat();

//打开文件系统
bool OpenFileSystem();

//显示磁盘的信息
void ShowDiskAttrib();

//将新建目录保存到磁盘
bool SaveFile(Directory &dir);

//将新建或修改文件保存到磁盘
bool SaveFile(File &file, const char *filecontest, bool isnew);

//更新目录
bool UpdateDirectory(Directory &dir);

//更新文件
bool UpdateFile(File &file);

//更新空闲空间管理块
void UpdateImt();

//更新超级块
void UpdateSuperBlock();

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//LogicFile.h

//复制一个文件到指定目录，如果不能复制，则返回false
bool NewFile(File oldfile, Directory &cur);

//根据指定父目录和目录名新建文件，如果不能新建，则返回false
bool NewDirectory(string dirname, Directory &cur);

//根据指定目录和文件名新建文件，如果不能新建，则返回false
bool NewFile(string filename, Directory &cur);

//显示目录信息
void showDirInfo(Directory dir, bool iscur);

//显示文件信息
void showFileInfo(File file);

//将当前目录定位到指定目录的父目录
bool UpDir(Directory &cur);

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//DirectorySearch.h

//根据指定的绝对路径找到相应目录
short FindDirInAbsPath(string filepath);

//根据指定的绝对路径找到相应文件
short FindFileInAbsPath(string filepath);

//比较两个目录是否是同一个目录
bool dircmp(Directory *dir1, Directory *dir2);

//根据给定的目录名在指定的目录下查找目录
short FindDirInDir(const char *dirname, Directory cur);

//根据给定的目录名在指定的目录下查找目录
short FindDirInDir(string dirname, Directory cur);

//根据给定的文件名在指定的目录下查找文件
short FindFileInDir(const char *filename, Directory cur);

//根据给定的文件名在指定的目录下查找文件
short FindFileInDir(string filename, Directory cur);

//列出给定目录下的子目录和文件信息
void ListDir(Directory dir);

//判断要新建的文件是否与当前目录下的目录或文件名冲突
bool NameConflict(char *filename, Directory cur);

//在命令行界面显示当前目录以提示用户
void PromptCurDir(Directory dir);			

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//Command.h

//进入目录
bool Cd(string args);

//判断当前目录是否是根目录，若是根目录，则返回false，不能再返回上一级目录；否则返回true
bool CdDot();


bool CmdWithoutArgs(string cmd);


bool Cp(string args1, string args2);

//dir(显示目录信息)命令处理函数
bool Dir(string dirname);

//显示所有命令
bool Help();

//显示命令的具体解析
bool Help(string args);

bool Md(string args);

//mk(新建文件)命令处理函数
bool Mk(string args);

//mkdir(新建目录)命令处理函数
bool MkDir(string args, Directory &dir);


bool Move(string args1,string args2);

//修改文件内容
bool Revise(string args);

//文件重新命名
bool Rename(string args);

//命令解析函数，解析输入的一串命令，以确定应该执行什么
bool ResolveCommand(string cmd);

//删除指定文件
bool Rm(File file);

//在给定的目录下,根据指定文件名删除文件
bool Rm(string args, Directory dir);

//根据指定文件名(可以是绝对路径)删除文件
bool Rm(string args);

//删除指定目录
bool Rmdir(Directory dir);

//根据指定目录名删除目录
bool Rmdir(string args);

//将输入的一串命令解析成命令名和参数两部分
void SplitCmdArgs(string &cmd,string &cmdname,string &args);

//试图将命令参数拆分成两部分
bool SplitArgs(string &args, string &args1, string &args2);

//显示文件内容
bool Type(string args);

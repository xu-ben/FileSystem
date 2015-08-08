/*
 * 文件名：		DirectorySearch.h 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	包含头文件中包含Definition.h，
 *				结构定义和变量定义以及对头文件PhysicFile.h、LogicFile.h、
 *				Command.h、DirectorySearch.h中定义的函数进行函数声明
 * 修改记录：	2010/06/04 
 */

#pragma once

#include "Definition.h"

//列出给定目录下的子目录和文件信息
void ListDir(Directory dir)
{
	showDirInfo(dir, true);
	
	for(int i = 0; i < dir.childdirnum; i++)
	{
		Directory d = getDirectory(dir.chidirs[i]);
		showDirInfo(d, dircmp(&d, &curdir));
	}

	for(int j = 0; j < dir.filenum; j++)
	{
		showFileInfo(getFile(dir.chifiles[j]));
	}

	cout<<"\t\t当前目录下共有\t"<<dir.filenum<<"\t个文件"<<endl;
	cout<<"\t\t\t\t"<<dir.childdirnum<<"\t个子目录"<<endl<<endl;
}

//在命令行界面显示当前目录以提示用户
void PromptCurDir(Directory dir)
{
	if(dir.fatherindex < 0)//根目录
	{
		cout<<dir.dirname<<":";
		return ;
	}

	PromptCurDir(getDirectory(dir.fatherindex));//递归调用函数输出

	cout<<"\\"<<dir.dirname;
}

//比较两个目录是否是同一个目录
bool dircmp(Directory *dir1, Directory *dir2)
{
	if((dir1->fatherindex == dir2->fatherindex) && (strcmp(dir1->dirname, dir2->dirname) == 0) && dir1->ownindex == dir2->ownindex)
		return true;
	else
		return false;
}

//判断要新建的文件是否与当前目录下的目录或文件名冲突
bool NameConflict(char *filename, Directory cur)
{
	if(strcmp(filename, "A:") == 0)
		return false;
	int i;
	for(i = 0; i < cur.childdirnum; i++)
	{
		if(strcmp(filename, getDirectory(cur.chidirs[i]).dirname) == 0)
			return false;
	}

	for(i = 0; i <cur.filenum; i++)
	{
		if(strcmp(filename, getFile(cur.chifiles[i]).filename) ==0)
			return false;
	}

	return true;
}

//根据给定的文件名在指定的目录下查找文件
short FindFileInDir(string filename, Directory cur)
{
	filename = Trim(filename);
	int index = filename.find_first_of("\\");
	if(index < 0)
	{
		const char *name = filename.c_str();
		return FindFileInDir(name, cur);
	}
	else
	{
		string dirname = filename.substr(0, index);
		short dirblocknum = FindDirInDir(dirname, cur);
		if(dirblocknum < 0)
			return -1;

		Directory dir = getDirectory(dirblocknum);

		return FindFileInDir(filename.substr(index + 1), dir);
	}
}

//根据指定的绝对路径找到相应文件
short FindFileInAbsPath(string filepath)
{
	filepath = Trim(filepath);

	if(filepath.length() <= 3)
		return -1;
	else if(filepath[0] == 'A' && filepath[1] == ':' && filepath[2] == '\\')
	{
		return FindFileInDir(filepath.substr(3), rootdir);
	}
	else
	{
		return -1;
	}
}

//根据指定的绝对路径找到相应目录
short FindDirInAbsPath(string filepath)
{
	filepath = Trim(filepath);

	if(filepath.compare("A:") == 0)
		return rootdir.ownindex;
	else if(filepath.length() <= 3)
		return -1;
	else if(filepath[0] == 'A' && filepath[1] == ':' && filepath[2] == '\\')
	{
		return FindDirInDir(filepath.substr(3), rootdir);
	}
	else
	{
		return -1;
	}
}

//根据给定的目录名在指定的目录下查找目录
short FindDirInDir(string dirname, Directory cur)
{
	dirname = Trim(dirname);

	int index = dirname.find_first_of("\\");
	if(index < 0)
	{
		const char *name = dirname.c_str();
		return FindDirInDir(name, cur);
	}
	else
	{
		string fdirname = dirname.substr(0, index);
		short dirblocknum = FindDirInDir(fdirname, cur);
		if(dirblocknum < 0)
			return -1;

		Directory dir = getDirectory(dirblocknum);

		return FindDirInDir(dirname.substr(index + 1), dir);
	}
}

//根据给定的文件名在指定的目录下查找文件
short FindFileInDir(const char *filename, Directory cur)
{
	for(int i = 0; i < cur.filenum; i++)
	{
		if(strcmp(filename, getFile(cur.chifiles[i]).filename) == 0)
		{
			return cur.chifiles[i];
		}
	}

	return -1;
}

//根据给定的目录名在指定的目录下查找目录
short FindDirInDir(const char *dirname, Directory cur)
{
	for(int i = 0; i < cur.childdirnum; i++)
	{
		if(strcmp(dirname, getDirectory(cur.chidirs[i]).dirname) == 0)
		{
			return cur.chidirs[i];
		}
	}

	return -1;
}

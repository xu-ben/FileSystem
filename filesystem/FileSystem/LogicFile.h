/*
 * 文件名：		LogicFile.h 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	包含头文件Definition.h和Tools.h，实现将当前目录定位到指定目录的父目录函数、
 *				显示目录信息函数、显示文件信息函数、根据指定父目录和目录名新建文件函数、
 *				根据指定目录和文件名新建文件函数。
 * 修改记录：	2010/06/04 
 */

#pragma once

#include "Definition.h"
#include "Tools.h"

//将当前目录定位到指定目录的父目录
bool UpDir(Directory &cur)
{
	if(cur.fatherindex < 0)
	{
		//cout<<"当前目录已经是根目录，不能再返回上级目录"<<endl;
		return false;
	}

	Directory father = getDirectory(cur.fatherindex);
	cur = father;
	return true;
}

//显示目录信息
void showDirInfo(Directory dir, bool iscur)
{
	if(iscur)
	{
		cout<<stroftime(&dir.create_time)<<"\t<DIR>\t\t."<<endl;
	}
	else
	{
		cout<<stroftime(&dir.create_time)<<"\t<DIR>\t\t"<<dir.dirname<<endl;
	}
}

//显示文件信息
void showFileInfo(File file)
{
	cout<<stroftime(&file.last_update_time)<<"\t\t"<<file.filesize<<"\t"<<file.filename<<endl;
}

//根据指定父目录和目录名新建文件，如果不能新建，则返回false
bool NewDirectory(string dirname, Directory &cur)
{
	if(cur.filenum >= FILE_NUM)
	{
		cout<<"当前目录下的文件数量已经达到最大值，无法新建文件!"<<endl;
		return false;
	}

	Directory dir;

	strcpy(dir.dirname, dirname.c_str());
	if(!NameConflict(dir.dirname, cur))
	{
		cout<<"目录名冲突，无法新建目录!"<<endl;
		return false;
	}

	dir.fatherindex = cur.ownindex;

	dir.filestyle = DIR; //FileStyle::

	dir.childdirnum = 0;
	dir.filenum = 0;

	if(!SaveFile(dir))
		return false;

	//将新建目录添加到父目录的列表中
	cur.chidirs[cur.childdirnum++] = dir.ownindex;

	UpdateDirectory(cur);//更新父目录

	return true;
}

//根据指定目录和文件名新建文件，如果不能新建，则返回false
bool NewFile(string filename, Directory &cur)
{
	if(cur.childdirnum >= CHILD_DIR_NUM)
	{
		cout<<"当前目录下的文件数量已经达到最大值，无法新建文件!"<<endl;
		return false;
	}

	File newfile;

	strcpy(newfile.filename, filename.c_str());
	if(!NameConflict(newfile.filename, cur))
	{
		cout<<"文件名冲突，无法新建文件!"<<endl;
		return false;
	}

	newfile.fatherindex = cur.ownindex;

	newfile.filestyle = TXT; //FileStyle::

	string contest = string("");
	const char *filecontest = contest.c_str();

	char c;
	cout<<"是否往文件里添加内容(Y/N)?";
	cin>>c;
	cin.ignore();
	if(c == 'Y' || c == 'y')
	{
		char row[CHAR_NUM_IN_A_ROW];

		cout<<"请输入要添加的内容的行数"<<endl;

		int rownum = 0;
		cin>>rownum;
		cin.get();

		cout<<"请输入要添加的内容:"<<endl;

		for(int i = 0; i < rownum; i++)
		{
			cin.getline(row, CHAR_NUM_IN_A_ROW);
			contest += row;
			contest += "\n";
		}

		cout<<"添加内容成功，当前文件 "<<newfile.filename<<" 中的内容为:"<<endl;
		cout<<"********************************************************************************"<<endl;
		cout<<contest<<endl;
		cout<<"********************************************************************************"<<endl;
		filecontest = contest.c_str();
	}
	
	if(!SaveFile(newfile, filecontest, true))
		return false;

	//将文件添加到目录中
	cur.chifiles[cur.filenum++] = newfile.stablock;

	UpdateDirectory(cur);//更新父目录

	return true;
}

//复制一个文件到指定目录，如果不能复制，则返回false
bool NewFile(File oldfile, Directory &cur)
{
	if(cur.childdirnum >= CHILD_DIR_NUM)
	{
		cout<<"当前目录下的文件数量已经达到最大值，无法新建文件!"<<endl;
		return false;
	}

	File newfile;

	strcpy(newfile.filename, oldfile.filename);

	if(!NameConflict(newfile.filename, cur))
	{
		cout<<"文件名冲突，无法新建文件!"<<endl;
		return false;
	}

	newfile.create_time = oldfile.create_time;
	newfile.last_update_time = oldfile.last_update_time;
	newfile.filesize = oldfile.filesize;
	newfile.filestyle = newfile.filestyle;

	string contest = getContestOfFile(oldfile);
	const char *filecontest = contest.c_str();
	
	if(!SaveFile(newfile, filecontest, true))
		return false;

	//将文件添加到目录中
	cur.chifiles[cur.filenum++] = newfile.stablock;

	UpdateDirectory(cur);//更新父目录

	return true;
}

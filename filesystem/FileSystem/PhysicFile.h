/*
 * 文件名：		PhysicFile.h 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	包含头文件Definition.h，实现根据文件结构从虚拟磁盘取出文件内容函数、
 *				显示磁盘的信息函数、更新超级块函数、安装操作系统函数、更新空闲空间管理块函数、
 *				更新目录函数、更新文件函数、打开文件系统函数、关闭文件系统函数
 * 修改记录：	2010/06/04 
 */

#pragma once

#include "Definition.h"

//根据文件结构从虚拟磁盘取出文件内容
string getContestOfFile(File f)
{
	short block = f.stablock + 1;
	FileSystem.seekg(BLOCK_SIZE * block);
	int length = f.filesize - BLOCK_SIZE;
	char *ch = new char[length + 1];

	FileSystem.read(ch, length + 1);

	string result = string(ch);

	return result;
}

//磁盘碎片整理
void diskdfr()
{
}

//显示磁盘的信息
void ShowDiskAttrib()
{
	cout<<superblock.authorinfo<<endl;
	cout<<"磁盘物理信息:"<<endl;
	cout<<"磁盘总大小:\t"<<BLOCK_NUM * BLOCK_SIZE<<"\t字节"<<endl;
	cout<<"磁盘块大小:\t"<<BLOCK_SIZE<<"\t字节"<<endl;
	cout<<"磁盘块个数:\t"<<BLOCK_NUM<<"\t块"<<endl;
	int idle = 0;
	for(int i = 0; i < imt.idlblocknum; i++)
	{
		idle += imt.blocks[i].endblock - imt.blocks[i].startblock + 1;
	}
	cout<<"已使用磁盘块数:\t"<<BLOCK_NUM - idle<<"\t块"<<endl;
	cout<<"空闲磁盘块数:\t"<<idle<<"\t块"<<endl;
	cout<<endl<<endl;
}

//更新超级块
void UpdateSuperBlock()
{
	SuperBlock spblock;//超级块
	spblock.blocknum = BLOCK_NUM;
	spblock.blocksize = BLOCK_SIZE;
	memset(spblock.authorinfo, 0, BLOCK_SIZE - sizeof(int) * 2);
	strcpy(spblock.authorinfo, "File System [版本 1.0.0]\n版权所有 (c) 2010 Beijing Forestry University。保留所有权利。\n");

	FileSystem.seekp(BLOCK_SIZE);
	FileSystem.write((char *)&spblock, sizeof(SuperBlock));
	FileSystem.flush();
}

//安装操作系统
void installos()
{
	BootBlock btblock;	//引导块
	memset(btblock.systeminfo, 0, BLOCK_SIZE);
	strcpy(btblock.systeminfo, "已安装 MiniDos 操作系统！");

	FileSystem.seekp(0);
	FileSystem.write((char *)&btblock, sizeof(BootBlock));
	FileSystem.flush();
}

//更新空闲空间管理块
void UpdateImt()
{
	FileSystem.seekp(BLOCK_SIZE * 2);
	FileSystem.write((char *)&imt, sizeof(IdlManTbl));
}

//更新目录
bool UpdateDirectory(Directory &dir)
{
//	cout<<dir.dirname<<endl<<dir.ownindex<<endl;
//	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
//	cout<<dir.ownindex<<endl;
//	cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	FileSystem.seekp(BLOCK_SIZE * dir.ownindex);
	FileSystem.write((char *)&dir, sizeof(Directory));
//	FileSystem.write(filecontest, length + 1);
	return true;
}

//更新文件
bool UpdateFile(File &file)
{
	//修改最近修改时间
	time_t nowtime = time(NULL);
	file.last_update_time = *localtime(&nowtime); 

	//写入磁盘
	FileSystem.seekp(BLOCK_SIZE * file.stablock);
	FileSystem.write((char *)&file, sizeof(File));

	return true;
}

//磁盘格式化
bool DiskFormat()
{
	CloseFileSystem();

	fstream newfile;
	newfile.open(SYSTEM_FILE_NAME, ios::out|ios::binary);
	
	//写入空字符使文件系统大小为初始大小
	char blankchars[SYSTEM_SIZE];
	memset(blankchars, 0, SYSTEM_SIZE);
	newfile.write(blankchars, sizeof(blankchars));
	
	imt.idlblocknum = 1;
	imt.blocks[0].startblock = 7;
	imt.blocks[0].endblock = BLOCK_NUM - 1;

	rootdir.filestyle = DIR;	//FileStyle::dir;
	strcpy(rootdir.dirname, SYSTEM_FILE_NAME);
	rootdir.fatherindex = -1;		//无父目录
	rootdir.filenum = 0;
	rootdir.childdirnum = 0;
	rootdir.ownindex = 6;
	time_t nowtime = time(NULL);
	rootdir.create_time = *localtime(&nowtime);

	newfile.seekp(BLOCK_SIZE * 2);			//定位到空闲空间管理块
	newfile.write((char *)&imt, sizeof(IdlManTbl));
	newfile.seekp(BLOCK_SIZE * 6);			//定位到第6块
	newfile.write((char *)&rootdir, sizeof(Directory));

	newfile.flush();
	newfile.close();

	//以读写方式打开二进制文件
	FileSystem.open(SYSTEM_FILE_NAME, ios::in|ios::out|ios::binary);
	if(!FileSystem)
	{
		cout<<"打开文件系统出错!"<<endl;
		return false;
	}

	installos();
	UpdateSuperBlock();

	FileSystem.seekg(0);						//定位到文件头
	FileSystem.read((char *)&bootblock, sizeof(BootBlock));
	FileSystem.read((char *)&superblock, sizeof(SuperBlock));
	FileSystem.read((char *)&imt, sizeof(IdlManTbl));

	FileSystem.seekp(BLOCK_SIZE * 6);			//定位到第6块
	FileSystem.read((char *)&rootdir, sizeof(Directory));

	curdir = rootdir;

	cout<<"磁盘格式化完毕!"<<endl;
	return true;
}

//打开文件系统
bool OpenFileSystem()
{

	fstream temp;
	temp.open(SYSTEM_FILE_NAME, ios::in|ios::binary);

	//找不到已经存在的文件系统，则新建一个
	if(!temp)
	{
		temp.close();

		char c;
		cout<<"警告：在当前位置未找到文件系统，是否新建文件系统(Y/N)？"<<endl;
		cin>>c;
		if(c != 'Y' && c != 'y')
			return false;
		//清空键盘缓冲区
		cin.clear();
		cin.ignore(1,'\n');	
		system("cls");

		fstream newfile;
		newfile.open(SYSTEM_FILE_NAME, ios::out|ios::binary);
		
		//写入空字符使文件系统大小为初始大小
		char blankchars[SYSTEM_SIZE];
		memset(blankchars, 0, SYSTEM_SIZE);
		newfile.write(blankchars, sizeof(blankchars));


		BootBlock btblock;//引导块
		memset(btblock.systeminfo, 0, BLOCK_SIZE);
		strcpy(btblock.systeminfo, "此文件系统中暂未安装任何操作系统");

		SuperBlock spblock;//超级块
		spblock.blocknum = BLOCK_NUM;
		spblock.blocksize = BLOCK_SIZE;
		memset(spblock.authorinfo, 0, BLOCK_SIZE - sizeof(int) * 2);
		strcpy(spblock.authorinfo, "此文件系统由北京林业大学信息学院徐犇、辛娟娟和蒋春倩为操作系统课程设计而作，版权所有。");

		imt.idlblocknum = 1;
		imt.blocks[0].startblock = 7;
		imt.blocks[0].endblock = BLOCK_NUM - 1;

		rootdir.filestyle = DIR;	//FileStyle::dir;
		strcpy(rootdir.dirname, SYSTEM_FILE_NAME);
		rootdir.fatherindex = -1;		//无父目录
		rootdir.filenum = 0;
		rootdir.childdirnum = 0;
		rootdir.ownindex = 6;
		time_t nowtime = time(NULL);
		rootdir.create_time = *localtime(&nowtime);

		newfile.seekp(0);						//定位到文件头
		newfile.write((char *)&btblock, sizeof(BootBlock));
		newfile.write((char *)&spblock, sizeof(SuperBlock));
		newfile.write((char *)&imt, sizeof(IdlManTbl));
		newfile.seekp(BLOCK_SIZE * 6);			//定位到第6块
		newfile.write((char *)&rootdir, sizeof(Directory));

		newfile.flush();
		newfile.close();
	}
	else
		temp.close();
	
	//以读写方式打开二进制文件
	FileSystem.open(SYSTEM_FILE_NAME, ios::in|ios::out|ios::binary);
	if(!FileSystem)
	{
		cout<<"打开文件系统出错!"<<endl;
		return false;
	}

	FileSystem.seekg(0);						//定位到文件头
	FileSystem.read((char *)&bootblock, sizeof(BootBlock));
	FileSystem.read((char *)&superblock, sizeof(SuperBlock));
	FileSystem.read((char *)&imt, sizeof(IdlManTbl));

	FileSystem.seekp(BLOCK_SIZE * 6);			//定位到第6块
	FileSystem.read((char *)&rootdir, sizeof(Directory));

	curdir = rootdir;
	return true;
}

//关闭文件系统
bool CloseFileSystem()
{
	FileSystem.close();
	return true;
}

//根据块号从磁盘中取出一个目录
Directory getDirectory(short blockindex)
{
	FileSystem.seekg(BLOCK_SIZE * blockindex);
	Directory dir;
	FileSystem.read((char *)&dir, sizeof(Directory));
	return dir;
}

//根据块号从磁盘中取出一个文件
File getFile(short blockindex)
{
	FileSystem.seekg(BLOCK_SIZE * blockindex);
	File file;
	FileSystem.read((char *)&file, sizeof(File));
	return file;
}

//将新建目录保存到磁盘
bool SaveFile(Directory &dir)
{
	short blocknum = 1;//规定目录只占用一块磁盘块

	short temp = AllocSpace(blocknum);
	if(temp < 7)
	{
		cout<<"磁盘空间不够，无法为新建目录分配空间，新建目录失败!"<<endl;
		return false;
	}

	dir.ownindex = temp;

	time_t nowtime = time(NULL);
	dir.create_time = *localtime(&nowtime);

	//将新建目录写到磁盘
	FileSystem.seekp(BLOCK_SIZE * dir.ownindex);
	FileSystem.write((char *)&dir, sizeof(Directory));
	FileSystem.flush();

	return true;
}

//将修改文件保存到磁盘
bool SaveFile(File &file, const char *filecontest, bool isnew)
{
	int length = strlen(filecontest);
	file.filesize = BLOCK_SIZE + length;
	short blocknum = static_cast<short>(ceil((file.filesize * 1.0) / BLOCK_SIZE));
	if(!isnew)
	{

		FreeSpace(file.stablock, file.blocknum);//先释放空间

		short temp = AllocSpace(blocknum);
		if(temp < 7)
		{
			cout<<"磁盘空间不够，无法为修改后的文件分配空间，修改文件失败!"<<endl;
			return false;
		}

		file.stablock = temp;
		file.blocknum = blocknum;

		time_t nowtime = time(NULL);
		file.last_update_time = *localtime(&nowtime);

	}
	else
	{
		short temp = AllocSpace(blocknum);
		if(temp < 7)
		{
			cout<<"磁盘空间不够，无法为新建文件分配空间，新建文件失败!"<<endl;
			return false;
		}

		file.stablock = temp;
		file.blocknum = blocknum;

		time_t nowtime = time(NULL);
		file.create_time = *localtime(&nowtime);
		file.last_update_time = file.create_time;

	}

	FileSystem.seekp(BLOCK_SIZE * file.stablock);
	FileSystem.write((char *)&file, sizeof(File));
	FileSystem.write(filecontest, length + 1);
	FileSystem.flush();

	return true;
}

//释放磁盘空间
bool FreeSpace(short stablock, short blocknum)
{

	if(imt.idlblocknum >= ((BLOCK_SIZE * 4 - sizeof(short)) / sizeof(short) / 2))
		return false;

	imt.blocks[imt.idlblocknum].startblock = stablock;
	imt.blocks[imt.idlblocknum].endblock = stablock + blocknum - 1;
	imt.idlblocknum++;
	
	UpdateImt();//更新空闲管理区

	return true;
}

//分配磁盘空间
short AllocSpace(short blocknum)
{
	for(int i = 0; i < imt.idlblocknum; i++)
	{
		short region = imt.blocks[i].endblock - imt.blocks[i].startblock + 1;
		if(region >= blocknum)
		{
			if(region == blocknum)
			{
				for(int j = i + 1; j < imt.idlblocknum; j++)
				{
					imt.blocks[j-1] = imt.blocks[j];
				}
				imt.idlblocknum--;
			}
			else
			{
				imt.blocks[i].startblock += blocknum;
			}

			UpdateImt();//更新空闲管理区

			return imt.blocks[i].startblock - blocknum;
		}
	}

	return -1;
}

/*
 * 对磁盘进行分块，每块大小128字节，共240块，第0块为引导块，第1块为超级块，2 - 5块存放空闲空间管理表
 * 第6块存放根目录，7 - 239块存放其它目录和文件
 */
/*
 * �ļ�����		PhysicFile.h 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	����ͷ�ļ�Definition.h��ʵ�ָ����ļ��ṹ���������ȡ���ļ����ݺ�����
 *				��ʾ���̵���Ϣ���������³����麯������װ����ϵͳ���������¿��пռ����麯����
 *				����Ŀ¼�����������ļ����������ļ�ϵͳ�������ر��ļ�ϵͳ����
 * �޸ļ�¼��	2010/06/04 
 */

#pragma once

#include "Definition.h"

//�����ļ��ṹ���������ȡ���ļ�����
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

//������Ƭ����
void diskdfr()
{
}

//��ʾ���̵���Ϣ
void ShowDiskAttrib()
{
	cout<<superblock.authorinfo<<endl;
	cout<<"����������Ϣ:"<<endl;
	cout<<"�����ܴ�С:\t"<<BLOCK_NUM * BLOCK_SIZE<<"\t�ֽ�"<<endl;
	cout<<"���̿��С:\t"<<BLOCK_SIZE<<"\t�ֽ�"<<endl;
	cout<<"���̿����:\t"<<BLOCK_NUM<<"\t��"<<endl;
	int idle = 0;
	for(int i = 0; i < imt.idlblocknum; i++)
	{
		idle += imt.blocks[i].endblock - imt.blocks[i].startblock + 1;
	}
	cout<<"��ʹ�ô��̿���:\t"<<BLOCK_NUM - idle<<"\t��"<<endl;
	cout<<"���д��̿���:\t"<<idle<<"\t��"<<endl;
	cout<<endl<<endl;
}

//���³�����
void UpdateSuperBlock()
{
	SuperBlock spblock;//������
	spblock.blocknum = BLOCK_NUM;
	spblock.blocksize = BLOCK_SIZE;
	memset(spblock.authorinfo, 0, BLOCK_SIZE - sizeof(int) * 2);
	strcpy(spblock.authorinfo, "File System [�汾 1.0.0]\n��Ȩ���� (c) 2010 Beijing Forestry University����������Ȩ����\n");

	FileSystem.seekp(BLOCK_SIZE);
	FileSystem.write((char *)&spblock, sizeof(SuperBlock));
	FileSystem.flush();
}

//��װ����ϵͳ
void installos()
{
	BootBlock btblock;	//������
	memset(btblock.systeminfo, 0, BLOCK_SIZE);
	strcpy(btblock.systeminfo, "�Ѱ�װ MiniDos ����ϵͳ��");

	FileSystem.seekp(0);
	FileSystem.write((char *)&btblock, sizeof(BootBlock));
	FileSystem.flush();
}

//���¿��пռ�����
void UpdateImt()
{
	FileSystem.seekp(BLOCK_SIZE * 2);
	FileSystem.write((char *)&imt, sizeof(IdlManTbl));
}

//����Ŀ¼
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

//�����ļ�
bool UpdateFile(File &file)
{
	//�޸�����޸�ʱ��
	time_t nowtime = time(NULL);
	file.last_update_time = *localtime(&nowtime); 

	//д�����
	FileSystem.seekp(BLOCK_SIZE * file.stablock);
	FileSystem.write((char *)&file, sizeof(File));

	return true;
}

//���̸�ʽ��
bool DiskFormat()
{
	CloseFileSystem();

	fstream newfile;
	newfile.open(SYSTEM_FILE_NAME, ios::out|ios::binary);
	
	//д����ַ�ʹ�ļ�ϵͳ��СΪ��ʼ��С
	char blankchars[SYSTEM_SIZE];
	memset(blankchars, 0, SYSTEM_SIZE);
	newfile.write(blankchars, sizeof(blankchars));
	
	imt.idlblocknum = 1;
	imt.blocks[0].startblock = 7;
	imt.blocks[0].endblock = BLOCK_NUM - 1;

	rootdir.filestyle = DIR;	//FileStyle::dir;
	strcpy(rootdir.dirname, SYSTEM_FILE_NAME);
	rootdir.fatherindex = -1;		//�޸�Ŀ¼
	rootdir.filenum = 0;
	rootdir.childdirnum = 0;
	rootdir.ownindex = 6;
	time_t nowtime = time(NULL);
	rootdir.create_time = *localtime(&nowtime);

	newfile.seekp(BLOCK_SIZE * 2);			//��λ�����пռ�����
	newfile.write((char *)&imt, sizeof(IdlManTbl));
	newfile.seekp(BLOCK_SIZE * 6);			//��λ����6��
	newfile.write((char *)&rootdir, sizeof(Directory));

	newfile.flush();
	newfile.close();

	//�Զ�д��ʽ�򿪶������ļ�
	FileSystem.open(SYSTEM_FILE_NAME, ios::in|ios::out|ios::binary);
	if(!FileSystem)
	{
		cout<<"���ļ�ϵͳ����!"<<endl;
		return false;
	}

	installos();
	UpdateSuperBlock();

	FileSystem.seekg(0);						//��λ���ļ�ͷ
	FileSystem.read((char *)&bootblock, sizeof(BootBlock));
	FileSystem.read((char *)&superblock, sizeof(SuperBlock));
	FileSystem.read((char *)&imt, sizeof(IdlManTbl));

	FileSystem.seekp(BLOCK_SIZE * 6);			//��λ����6��
	FileSystem.read((char *)&rootdir, sizeof(Directory));

	curdir = rootdir;

	cout<<"���̸�ʽ�����!"<<endl;
	return true;
}

//���ļ�ϵͳ
bool OpenFileSystem()
{

	fstream temp;
	temp.open(SYSTEM_FILE_NAME, ios::in|ios::binary);

	//�Ҳ����Ѿ����ڵ��ļ�ϵͳ�����½�һ��
	if(!temp)
	{
		temp.close();

		char c;
		cout<<"���棺�ڵ�ǰλ��δ�ҵ��ļ�ϵͳ���Ƿ��½��ļ�ϵͳ(Y/N)��"<<endl;
		cin>>c;
		if(c != 'Y' && c != 'y')
			return false;
		//��ռ��̻�����
		cin.clear();
		cin.ignore(1,'\n');	
		system("cls");

		fstream newfile;
		newfile.open(SYSTEM_FILE_NAME, ios::out|ios::binary);
		
		//д����ַ�ʹ�ļ�ϵͳ��СΪ��ʼ��С
		char blankchars[SYSTEM_SIZE];
		memset(blankchars, 0, SYSTEM_SIZE);
		newfile.write(blankchars, sizeof(blankchars));


		BootBlock btblock;//������
		memset(btblock.systeminfo, 0, BLOCK_SIZE);
		strcpy(btblock.systeminfo, "���ļ�ϵͳ����δ��װ�κβ���ϵͳ");

		SuperBlock spblock;//������
		spblock.blocknum = BLOCK_NUM;
		spblock.blocksize = BLOCK_SIZE;
		memset(spblock.authorinfo, 0, BLOCK_SIZE - sizeof(int) * 2);
		strcpy(spblock.authorinfo, "���ļ�ϵͳ�ɱ�����ҵ��ѧ��ϢѧԺ��ġ������ͽ���ٻΪ����ϵͳ�γ���ƶ�������Ȩ���С�");

		imt.idlblocknum = 1;
		imt.blocks[0].startblock = 7;
		imt.blocks[0].endblock = BLOCK_NUM - 1;

		rootdir.filestyle = DIR;	//FileStyle::dir;
		strcpy(rootdir.dirname, SYSTEM_FILE_NAME);
		rootdir.fatherindex = -1;		//�޸�Ŀ¼
		rootdir.filenum = 0;
		rootdir.childdirnum = 0;
		rootdir.ownindex = 6;
		time_t nowtime = time(NULL);
		rootdir.create_time = *localtime(&nowtime);

		newfile.seekp(0);						//��λ���ļ�ͷ
		newfile.write((char *)&btblock, sizeof(BootBlock));
		newfile.write((char *)&spblock, sizeof(SuperBlock));
		newfile.write((char *)&imt, sizeof(IdlManTbl));
		newfile.seekp(BLOCK_SIZE * 6);			//��λ����6��
		newfile.write((char *)&rootdir, sizeof(Directory));

		newfile.flush();
		newfile.close();
	}
	else
		temp.close();
	
	//�Զ�д��ʽ�򿪶������ļ�
	FileSystem.open(SYSTEM_FILE_NAME, ios::in|ios::out|ios::binary);
	if(!FileSystem)
	{
		cout<<"���ļ�ϵͳ����!"<<endl;
		return false;
	}

	FileSystem.seekg(0);						//��λ���ļ�ͷ
	FileSystem.read((char *)&bootblock, sizeof(BootBlock));
	FileSystem.read((char *)&superblock, sizeof(SuperBlock));
	FileSystem.read((char *)&imt, sizeof(IdlManTbl));

	FileSystem.seekp(BLOCK_SIZE * 6);			//��λ����6��
	FileSystem.read((char *)&rootdir, sizeof(Directory));

	curdir = rootdir;
	return true;
}

//�ر��ļ�ϵͳ
bool CloseFileSystem()
{
	FileSystem.close();
	return true;
}

//���ݿ�ŴӴ�����ȡ��һ��Ŀ¼
Directory getDirectory(short blockindex)
{
	FileSystem.seekg(BLOCK_SIZE * blockindex);
	Directory dir;
	FileSystem.read((char *)&dir, sizeof(Directory));
	return dir;
}

//���ݿ�ŴӴ�����ȡ��һ���ļ�
File getFile(short blockindex)
{
	FileSystem.seekg(BLOCK_SIZE * blockindex);
	File file;
	FileSystem.read((char *)&file, sizeof(File));
	return file;
}

//���½�Ŀ¼���浽����
bool SaveFile(Directory &dir)
{
	short blocknum = 1;//�涨Ŀ¼ֻռ��һ����̿�

	short temp = AllocSpace(blocknum);
	if(temp < 7)
	{
		cout<<"���̿ռ䲻�����޷�Ϊ�½�Ŀ¼����ռ䣬�½�Ŀ¼ʧ��!"<<endl;
		return false;
	}

	dir.ownindex = temp;

	time_t nowtime = time(NULL);
	dir.create_time = *localtime(&nowtime);

	//���½�Ŀ¼д������
	FileSystem.seekp(BLOCK_SIZE * dir.ownindex);
	FileSystem.write((char *)&dir, sizeof(Directory));
	FileSystem.flush();

	return true;
}

//���޸��ļ����浽����
bool SaveFile(File &file, const char *filecontest, bool isnew)
{
	int length = strlen(filecontest);
	file.filesize = BLOCK_SIZE + length;
	short blocknum = static_cast<short>(ceil((file.filesize * 1.0) / BLOCK_SIZE));
	if(!isnew)
	{

		FreeSpace(file.stablock, file.blocknum);//���ͷſռ�

		short temp = AllocSpace(blocknum);
		if(temp < 7)
		{
			cout<<"���̿ռ䲻�����޷�Ϊ�޸ĺ���ļ�����ռ䣬�޸��ļ�ʧ��!"<<endl;
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
			cout<<"���̿ռ䲻�����޷�Ϊ�½��ļ�����ռ䣬�½��ļ�ʧ��!"<<endl;
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

//�ͷŴ��̿ռ�
bool FreeSpace(short stablock, short blocknum)
{

	if(imt.idlblocknum >= ((BLOCK_SIZE * 4 - sizeof(short)) / sizeof(short) / 2))
		return false;

	imt.blocks[imt.idlblocknum].startblock = stablock;
	imt.blocks[imt.idlblocknum].endblock = stablock + blocknum - 1;
	imt.idlblocknum++;
	
	UpdateImt();//���¿��й�����

	return true;
}

//������̿ռ�
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

			UpdateImt();//���¿��й�����

			return imt.blocks[i].startblock - blocknum;
		}
	}

	return -1;
}

/*
 * �Դ��̽��зֿ飬ÿ���С128�ֽڣ���240�飬��0��Ϊ�����飬��1��Ϊ�����飬2 - 5���ſ��пռ�����
 * ��6���Ÿ�Ŀ¼��7 - 239��������Ŀ¼���ļ�
 */
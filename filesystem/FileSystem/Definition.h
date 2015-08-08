/*
 * �ļ�����		Definition.h 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	��ͷ�ļ��а����˱����������ͷ�ļ��������˺궨�壬
 *				�ṹ����ͱ��������Լ���ͷ�ļ�PhysicFile.h��LogicFile.h��
 *				Command.h��DirectorySearch.h�ж���ĺ������к�������
 * �޸ļ�¼��	2010/06/04 
 */

#pragma once

//////////////////////////////////////////////////////////////////
//��������Ҫ������ͷ�ļ�

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <windows.h>


using namespace std;


///////////////////////////////////////////////////////////////////
//�궨��
#define SYSTEM_SIZE	30720			//�ļ�ϵͳ��С��30K�ֽ�
#define SYSTEM_FILE_NAME	"A"		//ʵ�ʴ洢���ļ�ϵͳ���ļ�����

#ifndef OVERFLOW
#define OVERFLOW -2
#endif


#define CHAR_NUM_IN_A_ROW 250
#define BLOCK_SIZE	128				//���С(��λ:�ֽ�)
#define BLOCK_NUM	240				//������
#define DIR_NAME_LENGTH 24			//Ŀ¼������
#define FILE_NAME_LENGTH 40			//��ͨ�ļ�������
#define CHILD_DIR_NUM	12			//ÿ��Ŀ¼��ֱ����Ŀ¼���������
#define FILE_NUM	16				//ÿ��Ŀ¼��ֱ�Ӵ�ŵ��ļ����������

//////////////////////////////////////////////////////////////////
//�ṹ����ͱ�������

//�ļ�(Ŀ¼)���ͣ�dir��ʾĿ¼���������ֱַ��ʾ�������͵��ļ�
enum FileStyle		
{
	DIR,
	TXT,
	CPP,
	C,
	H
};

//������
struct BootBlock	
{
	char systeminfo[BLOCK_SIZE];
};

//������
struct SuperBlock	
{
	int blocknum;
	int blocksize;
	char authorinfo[BLOCK_SIZE - sizeof(int) * 2];
};

//����������
struct IdleBlock	
{
	short startblock;	//������������ʼ���
	short endblock;		//������������ֹ�������˿飩���
};

//���пռ�����
struct IdlManTbl
{
	short idlblocknum;//���п����
	IdleBlock blocks[(BLOCK_SIZE * 4 - sizeof(short)) / sizeof(short) / 2];//���п�����
};

//����Ŀ¼�ṹ��
struct Directory
{
	FileStyle filestyle;				//�ļ�(Ŀ¼)����
	short fatherindex;					//��Ŀ¼��ŵĿ��
	short ownindex;						//��Ŀ¼��ŵĿ��
	short childdirnum;					//��ǰĿ¼����Ŀ¼����
	short chidirs[CHILD_DIR_NUM];		//��ǰĿ¼����Ŀ¼�б�
	short filenum;						//��ǰĿ¼���ļ�����
	short chifiles[FILE_NUM];			//��ǰĿ¼�е��ļ��б�
	tm create_time;						//Ŀ¼����ʱ��
	char dirname[DIR_NAME_LENGTH];		//Ŀ¼��
};

//�����ļ��ṹ
struct File
{
	FileStyle filestyle;				//�ļ�(Ŀ¼)����
	short fatherindex;					//��Ŀ¼��ŵĿ��
	int   filesize;						//�ļ���С����λ���ֽڣ�
	short stablock;						//�ļ���ʼ���
	short blocknum;						//�ļ���ǰռ�ô��̿���
	tm create_time;						//�ļ�����ʱ��
	tm last_update_time;				//�ϴ��޸�ʱ��
	char filename[FILE_NAME_LENGTH];	//�ļ���
};

//�ļ���Ŀ¼�����壬���ļ���Ŀ¼����ͬ���Ľ��ӵ��ļ�Ŀ¼�ṹ��ȥ
union FileNode	
{
	File file;
	Directory dir;
};


fstream FileSystem;			//�����ļ���

IdlManTbl imt;				//���п�����

BootBlock bootblock;		//�������������

SuperBlock superblock;      //���峬�������

Directory rootdir;			//��Ŀ¼

Directory curdir;			//��ǰĿ¼

//////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////
//��������


//////////////////////////////////////////////////////////////////
//PhysicFile.h

//������̿ռ�
short AllocSpace(short blocknum);

//�ر��ļ�ϵͳ
bool CloseFileSystem();

//������Ƭ����
void diskdfr();

//�ͷŴ��̿ռ�
bool FreeSpace(short stablock, short blocknum);

//���ݿ�ŴӴ�����ȡ��һ��Ŀ¼
Directory getDirectory(short blockindex);

//�����ļ��ṹ���������ȡ���ļ�����
string getContestOfFile(File f);

//���ݿ�ŴӴ�����ȡ��һ���ļ�
File getFile(short blockindex);

//��װ����ϵͳ
void installos();

//���̸�ʽ��
bool DiskFormat();

//���ļ�ϵͳ
bool OpenFileSystem();

//��ʾ���̵���Ϣ
void ShowDiskAttrib();

//���½�Ŀ¼���浽����
bool SaveFile(Directory &dir);

//���½����޸��ļ����浽����
bool SaveFile(File &file, const char *filecontest, bool isnew);

//����Ŀ¼
bool UpdateDirectory(Directory &dir);

//�����ļ�
bool UpdateFile(File &file);

//���¿��пռ�����
void UpdateImt();

//���³�����
void UpdateSuperBlock();

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//LogicFile.h

//����һ���ļ���ָ��Ŀ¼��������ܸ��ƣ��򷵻�false
bool NewFile(File oldfile, Directory &cur);

//����ָ����Ŀ¼��Ŀ¼���½��ļ�����������½����򷵻�false
bool NewDirectory(string dirname, Directory &cur);

//����ָ��Ŀ¼���ļ����½��ļ�����������½����򷵻�false
bool NewFile(string filename, Directory &cur);

//��ʾĿ¼��Ϣ
void showDirInfo(Directory dir, bool iscur);

//��ʾ�ļ���Ϣ
void showFileInfo(File file);

//����ǰĿ¼��λ��ָ��Ŀ¼�ĸ�Ŀ¼
bool UpDir(Directory &cur);

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//DirectorySearch.h

//����ָ���ľ���·���ҵ���ӦĿ¼
short FindDirInAbsPath(string filepath);

//����ָ���ľ���·���ҵ���Ӧ�ļ�
short FindFileInAbsPath(string filepath);

//�Ƚ�����Ŀ¼�Ƿ���ͬһ��Ŀ¼
bool dircmp(Directory *dir1, Directory *dir2);

//���ݸ�����Ŀ¼����ָ����Ŀ¼�²���Ŀ¼
short FindDirInDir(const char *dirname, Directory cur);

//���ݸ�����Ŀ¼����ָ����Ŀ¼�²���Ŀ¼
short FindDirInDir(string dirname, Directory cur);

//���ݸ������ļ�����ָ����Ŀ¼�²����ļ�
short FindFileInDir(const char *filename, Directory cur);

//���ݸ������ļ�����ָ����Ŀ¼�²����ļ�
short FindFileInDir(string filename, Directory cur);

//�г�����Ŀ¼�µ���Ŀ¼���ļ���Ϣ
void ListDir(Directory dir);

//�ж�Ҫ�½����ļ��Ƿ��뵱ǰĿ¼�µ�Ŀ¼���ļ�����ͻ
bool NameConflict(char *filename, Directory cur);

//�������н�����ʾ��ǰĿ¼����ʾ�û�
void PromptCurDir(Directory dir);			

//////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//Command.h

//����Ŀ¼
bool Cd(string args);

//�жϵ�ǰĿ¼�Ƿ��Ǹ�Ŀ¼�����Ǹ�Ŀ¼���򷵻�false�������ٷ�����һ��Ŀ¼�����򷵻�true
bool CdDot();


bool CmdWithoutArgs(string cmd);


bool Cp(string args1, string args2);

//dir(��ʾĿ¼��Ϣ)�������
bool Dir(string dirname);

//��ʾ��������
bool Help();

//��ʾ����ľ������
bool Help(string args);

bool Md(string args);

//mk(�½��ļ�)�������
bool Mk(string args);

//mkdir(�½�Ŀ¼)�������
bool MkDir(string args, Directory &dir);


bool Move(string args1,string args2);

//�޸��ļ�����
bool Revise(string args);

//�ļ���������
bool Rename(string args);

//����������������������һ�������ȷ��Ӧ��ִ��ʲô
bool ResolveCommand(string cmd);

//ɾ��ָ���ļ�
bool Rm(File file);

//�ڸ�����Ŀ¼��,����ָ���ļ���ɾ���ļ�
bool Rm(string args, Directory dir);

//����ָ���ļ���(�����Ǿ���·��)ɾ���ļ�
bool Rm(string args);

//ɾ��ָ��Ŀ¼
bool Rmdir(Directory dir);

//����ָ��Ŀ¼��ɾ��Ŀ¼
bool Rmdir(string args);

//�������һ������������������Ͳ���������
void SplitCmdArgs(string &cmd,string &cmdname,string &args);

//��ͼ�����������ֳ�������
bool SplitArgs(string &args, string &args1, string &args2);

//��ʾ�ļ�����
bool Type(string args);

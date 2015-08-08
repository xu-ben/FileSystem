/*
 * �ļ�����		DirectorySearch.h 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	����ͷ�ļ��а���Definition.h��
 *				�ṹ����ͱ��������Լ���ͷ�ļ�PhysicFile.h��LogicFile.h��
 *				Command.h��DirectorySearch.h�ж���ĺ������к�������
 * �޸ļ�¼��	2010/06/04 
 */

#pragma once

#include "Definition.h"

//�г�����Ŀ¼�µ���Ŀ¼���ļ���Ϣ
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

	cout<<"\t\t��ǰĿ¼�¹���\t"<<dir.filenum<<"\t���ļ�"<<endl;
	cout<<"\t\t\t\t"<<dir.childdirnum<<"\t����Ŀ¼"<<endl<<endl;
}

//�������н�����ʾ��ǰĿ¼����ʾ�û�
void PromptCurDir(Directory dir)
{
	if(dir.fatherindex < 0)//��Ŀ¼
	{
		cout<<dir.dirname<<":";
		return ;
	}

	PromptCurDir(getDirectory(dir.fatherindex));//�ݹ���ú������

	cout<<"\\"<<dir.dirname;
}

//�Ƚ�����Ŀ¼�Ƿ���ͬһ��Ŀ¼
bool dircmp(Directory *dir1, Directory *dir2)
{
	if((dir1->fatherindex == dir2->fatherindex) && (strcmp(dir1->dirname, dir2->dirname) == 0) && dir1->ownindex == dir2->ownindex)
		return true;
	else
		return false;
}

//�ж�Ҫ�½����ļ��Ƿ��뵱ǰĿ¼�µ�Ŀ¼���ļ�����ͻ
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

//���ݸ������ļ�����ָ����Ŀ¼�²����ļ�
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

//����ָ���ľ���·���ҵ���Ӧ�ļ�
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

//����ָ���ľ���·���ҵ���ӦĿ¼
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

//���ݸ�����Ŀ¼����ָ����Ŀ¼�²���Ŀ¼
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

//���ݸ������ļ�����ָ����Ŀ¼�²����ļ�
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

//���ݸ�����Ŀ¼����ָ����Ŀ¼�²���Ŀ¼
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

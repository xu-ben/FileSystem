/*
 * �ļ�����		FileSystem.cpp 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	�������������������ͨ������������������ɳ�ʼ��������
 *				����ļ�ϵͳ�����ÿ���̨���������ɫ�������û�����
 *				�Ա���д��������˳�ʱ�ر�ϵͳ
 * �޸ļ�¼��	2010/06/04 
 */

#include "Definition.h"
#include "Tools.h"
#include "PhysicFile.h"
#include "Command.h"
#include "DirectorySearch.h"
#include "LogicFile.h"

int main()
{
	setPrintColor(10);				//������̨���������ɫ�ĳ���ɫ

	if(!OpenFileSystem())
	{
		cout<<"���ļ�ϵͳʧ�ܣ����򼴽��˳���"<<endl;
		return 0;
	}

	cout<<superblock.authorinfo<<endl;

	char temp[80];

//	cout<<sizeof(FileNode)<<endl<<endl;
//	cout<<sizeof(short)<<endl<<endl;
//	cout<<sizeof(char)<<endl<<endl;
//	cout<<sizeof(int)<<endl<<endl;
//	cout<<sizeof(long)<<endl<<endl;
//	cout<<sizeof(Directory)<<endl;

	while(1)
	{

		PromptCurDir(curdir);
		if(dircmp(&curdir, &rootdir))
			cout<<"\\>";
		else
			cout<<">";

		cin.getline(temp, 80);

		string cmd = string(temp);

		if(!ResolveCommand(cmd))
			break;
	}

	CloseFileSystem();

	return 0;
}

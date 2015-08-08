/*
 * 文件名：		FileSystem.cpp 
 * 编写者：		ben
 * 编写日期：	2010/05/30
 * 简要描述：	包含本程序的主函数，通过调用其它函数，完成初始化工作，
 *				如打开文件系统，设置控制台输出文字颜色，接收用户输入
 *				以便进行处理，程序退出时关闭系统
 * 修改记录：	2010/06/04 
 */

#include "Definition.h"
#include "Tools.h"
#include "PhysicFile.h"
#include "Command.h"
#include "DirectorySearch.h"
#include "LogicFile.h"

int main()
{
	setPrintColor(10);				//将控制台输出文字颜色改成绿色

	if(!OpenFileSystem())
	{
		cout<<"打开文件系统失败，程序即将退出！"<<endl;
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

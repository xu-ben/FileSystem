/*
 * �ļ�����		LogicFile.h 
 * ��д�ߣ�		ben
 * ��д���ڣ�	2010/05/30
 * ��Ҫ������	����ͷ�ļ�Definition.h��Tools.h��ʵ�ֽ���ǰĿ¼��λ��ָ��Ŀ¼�ĸ�Ŀ¼������
 *				��ʾĿ¼��Ϣ��������ʾ�ļ���Ϣ����������ָ����Ŀ¼��Ŀ¼���½��ļ�������
 *				����ָ��Ŀ¼���ļ����½��ļ�������
 * �޸ļ�¼��	2010/06/04 
 */

#pragma once

#include "Definition.h"
#include "Tools.h"

//����ǰĿ¼��λ��ָ��Ŀ¼�ĸ�Ŀ¼
bool UpDir(Directory &cur)
{
	if(cur.fatherindex < 0)
	{
		//cout<<"��ǰĿ¼�Ѿ��Ǹ�Ŀ¼�������ٷ����ϼ�Ŀ¼"<<endl;
		return false;
	}

	Directory father = getDirectory(cur.fatherindex);
	cur = father;
	return true;
}

//��ʾĿ¼��Ϣ
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

//��ʾ�ļ���Ϣ
void showFileInfo(File file)
{
	cout<<stroftime(&file.last_update_time)<<"\t\t"<<file.filesize<<"\t"<<file.filename<<endl;
}

//����ָ����Ŀ¼��Ŀ¼���½��ļ�����������½����򷵻�false
bool NewDirectory(string dirname, Directory &cur)
{
	if(cur.filenum >= FILE_NUM)
	{
		cout<<"��ǰĿ¼�µ��ļ������Ѿ��ﵽ���ֵ���޷��½��ļ�!"<<endl;
		return false;
	}

	Directory dir;

	strcpy(dir.dirname, dirname.c_str());
	if(!NameConflict(dir.dirname, cur))
	{
		cout<<"Ŀ¼����ͻ���޷��½�Ŀ¼!"<<endl;
		return false;
	}

	dir.fatherindex = cur.ownindex;

	dir.filestyle = DIR; //FileStyle::

	dir.childdirnum = 0;
	dir.filenum = 0;

	if(!SaveFile(dir))
		return false;

	//���½�Ŀ¼��ӵ���Ŀ¼���б���
	cur.chidirs[cur.childdirnum++] = dir.ownindex;

	UpdateDirectory(cur);//���¸�Ŀ¼

	return true;
}

//����ָ��Ŀ¼���ļ����½��ļ�����������½����򷵻�false
bool NewFile(string filename, Directory &cur)
{
	if(cur.childdirnum >= CHILD_DIR_NUM)
	{
		cout<<"��ǰĿ¼�µ��ļ������Ѿ��ﵽ���ֵ���޷��½��ļ�!"<<endl;
		return false;
	}

	File newfile;

	strcpy(newfile.filename, filename.c_str());
	if(!NameConflict(newfile.filename, cur))
	{
		cout<<"�ļ�����ͻ���޷��½��ļ�!"<<endl;
		return false;
	}

	newfile.fatherindex = cur.ownindex;

	newfile.filestyle = TXT; //FileStyle::

	string contest = string("");
	const char *filecontest = contest.c_str();

	char c;
	cout<<"�Ƿ����ļ����������(Y/N)?";
	cin>>c;
	cin.ignore();
	if(c == 'Y' || c == 'y')
	{
		char row[CHAR_NUM_IN_A_ROW];

		cout<<"������Ҫ��ӵ����ݵ�����"<<endl;

		int rownum = 0;
		cin>>rownum;
		cin.get();

		cout<<"������Ҫ��ӵ�����:"<<endl;

		for(int i = 0; i < rownum; i++)
		{
			cin.getline(row, CHAR_NUM_IN_A_ROW);
			contest += row;
			contest += "\n";
		}

		cout<<"������ݳɹ�����ǰ�ļ� "<<newfile.filename<<" �е�����Ϊ:"<<endl;
		cout<<"********************************************************************************"<<endl;
		cout<<contest<<endl;
		cout<<"********************************************************************************"<<endl;
		filecontest = contest.c_str();
	}
	
	if(!SaveFile(newfile, filecontest, true))
		return false;

	//���ļ���ӵ�Ŀ¼��
	cur.chifiles[cur.filenum++] = newfile.stablock;

	UpdateDirectory(cur);//���¸�Ŀ¼

	return true;
}

//����һ���ļ���ָ��Ŀ¼��������ܸ��ƣ��򷵻�false
bool NewFile(File oldfile, Directory &cur)
{
	if(cur.childdirnum >= CHILD_DIR_NUM)
	{
		cout<<"��ǰĿ¼�µ��ļ������Ѿ��ﵽ���ֵ���޷��½��ļ�!"<<endl;
		return false;
	}

	File newfile;

	strcpy(newfile.filename, oldfile.filename);

	if(!NameConflict(newfile.filename, cur))
	{
		cout<<"�ļ�����ͻ���޷��½��ļ�!"<<endl;
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

	//���ļ���ӵ�Ŀ¼��
	cur.chifiles[cur.filenum++] = newfile.stablock;

	UpdateDirectory(cur);//���¸�Ŀ¼

	return true;
}

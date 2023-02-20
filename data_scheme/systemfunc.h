#pragma once


#include <iostream>
#include "MyHashTable.h"


using namespace std;
constexpr int define_print = -71717171;
constexpr int NIL = 0;

 
//Scheme interpreter ������ NIL=0 �̴� ���̺���
// ���� �̰� ����� �ٲ��� ���� !!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!! don't change NIL=0
//! 
//! 

constexpr int MAX_LEN = 11;
///// �ִ�  ��Ȳ���� �ٲ����� ��ǲ�� ���� �������


string SMpreProcessing(string newCommand);

bool isStrNum(string chk_str);

struct Pair{
	int idx;
	int val;
};

/*cpp ���� ���� ���� ���� ���� ����*/
class SMStack {
public:
	SMStack(int capa) 
		:mCapa(capa),mSize(0)
	{
		arr = new Pair[capa];
	}
	void push(Pair node) {
		if (mSize == mCapa) {
			
			Pair* tmp_arr = new Pair[2 * mCapa];
			for (int i = 0; i < mCapa; i++) {
				tmp_arr[i] = arr[i];
			}
			Pair* delete_arr = arr;
			arr = tmp_arr;
			delete arr;
		}
		arr[mSize++] = node;
	}
	Pair pop() {
		mSize--;
		return arr[mSize];
	}
	bool isEmpty() {
		return mSize == 0;
	}
	void flush() {
		mSize = 0;
	}
	~SMStack() {
		delete arr;
	}
private:
	Pair* arr;
	int mSize;
	int mCapa;
};


class StrBuf {
public:
	StrBuf(const string& str);
	inline char get();
	inline void putback(char c);
	inline string GetString();
private:
	string mStr;
};

string StrBuf::GetString() { return mStr; }


void Initalize(SMsystem& sm);
//init ������ ( , ) ������ �̸� �ؽ� ���̺� ä���ְ� ���� ����Ʈ�� �����.

string GetNextToken(StrBuf& is);

////getnexttoken ���� �ܼ��� token�� �޴´� .read �Լ����� ���
/// �ϳ��� ��ū�� ����� 10���϶�� �ؼ� size 11 �� char_array �������  ����!!!!
///// ���࿡ ������ ����ÿ� �ٲ��� 
///// is.get() �Լ��� is.putback() �Լ� ���
///// (, ) �� ���� \n ������ ����
//////example : (a + b + c) => ( , a , + ,b , + ,c )


/*if you give idx in hash it returns number*/
int SMgetVal(const int idx, SMsystem& sm);


int GetHash(const string& str, SMsystem& sm);
///GetHash �Լ��� �ɺ��� �ؽ����̺� ��ġ�� �����ָ鼭 �ؽ� ���̺� �־��ִ� ���� ���� �׸��� �� ��ġ�� �����Ѵ�.
////HashFunc���� �ؽ� �ް� -> ��ġ ã�� -> �� ��ġ�� ����
////�Ű� ������ str�� hashtable 



int SMeval(int root_idx,SMsystem& sm);



int Alloc(SMsystem& sm);

// Alloc �Լ����� freelist[0]�� ����Ʈ �� ��Ʈ��带 ��ȯ�Ѵ�.
// ���� ��밡���� �޸� �ݳ����ش� .
/// �Ű������� freelist 

int READ(StrBuf& is, int size_hash, SMsystem& sm);

/*READ �Լ�  input => ��ü ������ ����� very important function 
	!!!!!!!!
	Alloc GetHash GetNextToken ���
	�Ű������� �޸����̺� �ؽ����̺� ��������Ʈ , inputstream �־��ش� (ex. cin)

*/

/*READ ������ ��ū�� �а�
GetHash �� hash_value �� ���� ������
(init �� �̸� (, ) �� �־��ش�.)

bool first �� flag �Ҹ������� �̰� ����Ʈ�� ó������ �˷��ش�.
true �� root�� ���� false�� �̾ �Ѵ�.

tmp ���� ���� �޸𸮸� �־��ش�.
�̰� )�� ���� ������ �ϴµ�

���࿡ �߰��� ( �� ������ ��
another list�� �о�� �� ��쿡�� READ �� ��������� ���� (is.putback �Լ� ����Ͽ� ( �ٽ� ������)

READ �Լ��� ����Ʈ�� root index�� �Ҵ��ϱ� ������
��������� ��� ����
*/








void PRINT(int idx, bool start, SMsystem& sm);


/*PRINT function is just printing state

put in hash and memo  and root number and true 
�Ľ��� ������ print �Ѵ� 
pase tree ����
*/
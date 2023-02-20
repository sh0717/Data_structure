#pragma once
#include <string>
#include <memory>
#include <iostream>
using namespace std;

enum {
	FREE = -1,
	VARY=0,
	CONST=1
};


constexpr int HASH_NIL = 1111111111;

/*����ü Ŭ���� �����ε� �Լ����� ���� ����� �κ� ���� ����x */


extern unsigned int HashFunc_(const string& s);
//////memory hashtable capacity 31�̶�� 0~30���� �ִ� ���̴�. HashFunc_ �� string�� �ָ� hash�� ������ ������ func


/*StrBuf is just like inputstream use get �տ� ���� �ϳ� �̱� putback ���� �ϳ� �տ� �ֱ� */





/*Memory ���� ���   std::pair �ᵵ �ſ� ���� ��  �������� stl ���� �ᵵ ������ ������...*/
struct MemoryNode {
	int left=0;
	int right = 0;
};
/*Memory Table -----list information container

	there is constructor which get in for size 
	and it contains memory node array by uni_que ptr

	default_size =31; 
	�����ѹ� ���� �Ⱦ
	enum ������ ����ؼ� ����� ���� 
	static int �� �ٲٰ� �ʹٸ� �ٲ㵵 ����� ����

	operator[] �� �����ؼ� �ȿ� �ִ� table[]�� �ϵ��� �س��� 


*/

class MemoryTable {
public:
	MemoryTable();
	MemoryTable(int size);

	void SetMemory(int idx, int left, int right);
	MemoryNode& operator[](int idx) const;
	inline int GetSize() const;
	MemoryNode GetMemory(int idx);
	void check()const;
private:

	enum {default_size=31};
	int mCapacity;
	unique_ptr<MemoryNode[]> memorytable;
};

int MemoryTable::GetSize()const {
	return mCapacity;
};

/*HashNode ---HashTable �� ���� ��� std::pair<string,int>�� ����
	private ������ �س��� get,set �� ����� ���� �ߴµ�
	���� �̷��� ���� ���ص� �Ǳ������� �ϴ� 
	����� ���� �ٸ� ������ get,set ���� ������ �ٲ��� ����
	struct�� �ƴϱ⿡ operator= �����س��� 
	��������ڵ� 

	inline function ����߱� ������ header file ���� ������ ����!
*/

class HashNode {
public:
	HashNode();
	HashNode(string str, int val);
	HashNode(const HashNode& copy);

	HashNode& operator=(const HashNode& rhs);


	inline string GetString() const;
	inline int GetVal() const;
	inline void SetVal(int val);
private:
	string mStr;
	int mValue;
};


string HashNode::GetString() const { return mStr; };
int HashNode::GetVal() const { return mValue; };
void HashNode::SetVal(int val) { mValue = val; return; };



/*Memory Table �� ���� ���� almost same!!!*/

class MyHashTable
{
public:
	MyHashTable();
	MyHashTable(int size);
	inline int GetSize() const;
	

	void check() const;
	HashNode& operator[](int num) const;



private:
	enum {default_size=31};
	unique_ptr<HashNode[]> mTable;
	int mCapacity;
};

int MyHashTable::GetSize() const {
	return mCapacity;
};





/*SMsystem is wrapper function of hash and memo and freelist which have reference of that */

class  SMsystem
{
public:
	MyHashTable& hash;
	MemoryTable& memo;
	MemoryTable& freelist;

	SMsystem(MyHashTable& hash, MemoryTable& memo, MemoryTable& freelist) 
		:hash(hash),memo(memo),freelist(freelist),useCnt(0),memorySize(memo.GetSize())
	{
		state = new int[memorySize];
		for (int i = 0; i < memorySize; i++) {
			state[i] = FREE;
		}
	}

	SMsystem(const SMsystem& rhs) = delete;
	SMsystem& operator=(const SMsystem& rhs) = delete;
	~SMsystem() {
		delete[] state;
	}
	void GarbageCollect();

	void ChangeState(int idx, int stateNum) {
		state[idx] = stateNum;
		if (stateNum == CONST) {
			if (memo[idx].left > 0) {
				ChangeState(memo[idx].left, stateNum);
			}
			if (memo[idx].right > 0) {
				ChangeState(memo[idx].right, stateNum);
			}
		}
	}

	inline int GetUseCnt() const{
		return useCnt;
	}
	inline void AddUseCnt() {
		useCnt++;
	}
	inline void DecUseCnt() {
		useCnt--;
	}
	inline void SetGarbageFlag(bool am) {
		garbage_flag = am;
	}
	inline bool GetGarbageFlag() {
		return garbage_flag;
	}
	inline void SetDefineFlag(bool am) {
		define_flag = am;
	}
	inline bool GetDefineFlag() {
		return define_flag;
	}
	

private:
	bool garbage_flag = false;
	bool define_flag = false;
	int* state;
	int memorySize;
	int useCnt;
};

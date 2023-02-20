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

/*구조체 클래스 파일인데 함수에서 딱히 어려운 부분 없음 설명x */


extern unsigned int HashFunc_(const string& s);
//////memory hashtable capacity 31이라면 0~30까지 있는 것이다. HashFunc_ 는 string을 주면 hash를 내놓는 간단한 func


/*StrBuf is just like inputstream use get 앞에 문자 하나 뽑기 putback 문자 하나 앞에 넣기 */





/*Memory 단위 노드   std::pair 써도 매우 좋을 것  이정도는 stl 에서 써도 괜찮지 않을까...*/
struct MemoryNode {
	int left=0;
	int right = 0;
};
/*Memory Table -----list information container

	there is constructor which get in for size 
	and it contains memory node array by uni_que ptr

	default_size =31; 
	매직넘버 쓰기 싫어서
	enum 잡기술을 사용해서 만들어 놓음 
	static int 로 바꾸고 싶다면 바꿔도 상관은 없음

	operator[] 를 구현해서 안에 있는 table[]를 하도록 해놓음 


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

/*HashNode ---HashTable 의 단위 노드 std::pair<string,int>와 같음
	private 변수로 해놔서 get,set 을 만들어 놓긴 했는데
	굳이 이렇게 까지 안해도 되긴하지만 일단 
	만들어 놓음 다른 곳에서 get,set 쓰기 때문에 바꾸지 말자
	struct가 아니기에 operator= 구현해놓음 
	복사생성자도 

	inline function 사용했기 때문에 header file 에서 지우지 말것!
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



/*Memory Table 과 완전 동일 almost same!!!*/

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

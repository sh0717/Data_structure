#pragma once


#include <iostream>
#include "MyHashTable.h"


using namespace std;
constexpr int define_print = -71717171;
constexpr int NIL = 0;

 
//Scheme interpreter 에서는 NIL=0 이다 테이블에서
// 따라서 이것 절대로 바꾸지 말것 !!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!! don't change NIL=0
//! 
//! 

constexpr int MAX_LEN = 11;
///// 애는  상황보고 바꿀지도 인풋에 따라 고려사항


string SMpreProcessing(string newCommand);

bool isStrNum(string chk_str);

struct Pair{
	int idx;
	int val;
};

/*cpp 파일 설명 참조 제일 위에 있음*/
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
//init 에서는 ( , ) 같은것 미리 해쉬 테이블에 채워주고 프리 리스트를 만든다.

string GetNextToken(StrBuf& is);

////getnexttoken 으로 단순히 token을 받는다 .read 함수에서 사용
/// 하나의 토큰이 사이즈가 10이하라고 해서 size 11 의 char_array 사용했음  주의!!!!
///// 만약에 사이즈 변경시에 바꾸자 
///// is.get() 함수와 is.putback() 함수 사용
///// (, ) 나 공백 \n 등으로 구분
//////example : (a + b + c) => ( , a , + ,b , + ,c )


/*if you give idx in hash it returns number*/
int SMgetVal(const int idx, SMsystem& sm);


int GetHash(const string& str, SMsystem& sm);
///GetHash 함수는 심볼의 해쉬테이블 위치를 정해주면서 해쉬 테이블에 넣어주는 일을 하자 그리고 그 위치를 리턴한다.
////HashFunc에서 해쉬 받고 -> 위치 찾고 -> 그 위치를 리턴
////매개 변수로 str과 hashtable 



int SMeval(int root_idx,SMsystem& sm);



int Alloc(SMsystem& sm);

// Alloc 함수에서 freelist[0]의 라이트 즉 루트노드를 반환한다.
// 지금 사용가능한 메모리 반납해준다 .
/// 매개변수로 freelist 

int READ(StrBuf& is, int size_hash, SMsystem& sm);

/*READ 함수  input => 전체 구조를 만드는 very important function 
	!!!!!!!!
	Alloc GetHash GetNextToken 사용
	매개변수로 메모리테이블 해쉬테이블 프리리스트 , inputstream 넣어준다 (ex. cin)

*/

/*READ 에서는 토큰을 읽고
GetHash 로 hash_value 를 받은 다음에
(init 로 미리 (, ) 은 넣어준다.)

bool first 는 flag 불리언으로 이게 리스트의 처음인지 알려준다.
true 면 root를 변경 false면 이어서 한다.

tmp 에는 현재 메모리를 넣어준다.
이걸 )가 나올 때까지 하는데

만약에 중간에 ( 가 나오면 즉
another list를 읽어야 할 경우에는 READ 를 재귀적으로 실행 (is.putback 함수 사용하여 ( 다시 돌려줌)

READ 함수는 리스트의 root index를 할당하기 떄문에
재귀적으로 사용 가능
*/








void PRINT(int idx, bool start, SMsystem& sm);


/*PRINT function is just printing state

put in hash and memo  and root number and true 
파싱한 정보를 print 한다 
pase tree 따라서
*/
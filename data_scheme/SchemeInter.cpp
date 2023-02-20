
#include <stdio.h>
#include "MyHashTable.h"
#include <string>
#include "systemfunc.h"

/*hw 6 설명!!!!!!!
= 와 equal? eq?
키워드의 경우 미리 hashtable 에 추가 하고 
eval 에 작동 설명(eval 주석 최하단에 추가 !!)

garbage collector 의 경우에 

SMsystem 에 메모리의 상태를 저장하는 array 를 만들고 
(상태 1
    1.FREE
    2.VARY
    3.CONST 
)
FREE의 경우 아직 alloc 안된 메모리 
VARY의 경우 alloc 되었으나 garbage collector 작동시에 청소 가능한 쓰레기
CONST의 경우 alloc 되었고 garblage collect시에도 유지할 메모리

stat 이 바 뀔 경우는 
1.ALLOC 진행시 

2. define 으로 저장시

3. 청소시 

1.ALLOC때에는 FREE인 것을 하나 잡아와서(free list 에서 잡아옴)
stat을 FREE=> VARY로 저장

2.define 시에 
stat 을 VARY=> CONST로 변경 
define 결과 해쉬에 저장할 memory idx 의 state를 const 로 변경 
이 프로그램의 경우 delete 가 없기 때문에 간단하게 구현 가능 만약 delete 가 가능하면 
const 에서 다시 VAry 로 변경도 고려해야 한다. 

3. garbage 청소시

VARY인 것들을 FREE로 변경시키고 free list에 다시 넣는다.


근데 2번 상황에서 CONST 로 변경시에 
left나 right가 메모리일 시에 const 로 재귀적으로 change 해준다. 

ALLOC 을 하다가 가득 찼을 시에는 garbage collect를 작동시키며 eval 까지 작동하지 않고 다시 command 를 받는다.


MyhashTablle.cpp 파일 제일 아래쪽에 smsystem::garbagecollect함수 이용
*/




/* system func들은 systemfunc.h 에 존재하며 과제 1에서의 설명은 주로 헤더파일 과제 2 에서 추가된 펑션의 경우 cpp 파일에 설명이 있습니다. */
/*
Hash table 상에서 의 NIL 은 메모리상에서의 NIL과 구분된다. HASH_NIL
왜냐하면 만약에 두 공간에서 NIL을 0 으로 공유하면 
(define a '())
(null? a)
(null? b) 의 경우에 
a는 #t를 b는 오류를 내야 하는데 
만약에 
NIL을 공유한다면 

hash 상에서
string value 
"a"      NIL
"b"      NIL
로 할당되어 있어서 이게 공리스트로 할당되어 있는지 아니면 할당되지 않은 변수인지 구별할 수가 없다 그래서 hash 상에서 NIl 을 HASH_NIL로 구분한다면

"a"    NIL
"b"    HASH_NIL 이 되어서 
구분할 수가 있다. 



*/
/*
this file have main function 
overall explanation is in this file and 
function explanation is in other header file 

Structure is in MyHashTable.h and cpp file
and Functions for System is in systemfunc .h and cpp file


Scheme interpreter consists of 1.MemoryTable 2. HashTable 3.free_list

Sheme data was implemented in list structure 
MemoryTable is that list. It used array of  MemoryNode

Memory Node is just struct of two int_32;

and left int is for left side and right int is for right side 
if int is negative it means that  child is  in hash table and 
if int is positive it mens that child is another list

        --------------------------------
        |      left      |     right    |   <==========Memorytable[0] 실제로는 null부분
        |-------------------------------
        |                |              |   <==========Memorytable[1] 
        |________________________________
        |                |              |  
        |________________________________



HashTable is also similar it is implemented by array of HashNode
HashNode is class which have a string and a int 
int is value and 
string is string
it uses HashFunc(const string& input) in MyHashTable file
string is inverted to int and 
by incrementing index find empty hash_part and insert string 




Free_list is for finding Memory node which is not being used 
using MemoryTable structure 
right number is for connecting and 
node[i] is connected to i+1th node at first 

Root node is node which is connected by 0th node 
when allocating memory find free_list[0].right and allocate that and change free_list[0]

when  finishing parsing
use Returnmemory function 
다시 free_list 에 재귀적으로 연결시켜준다.




*/
using namespace std;

/*  hash_table ,이나 memory_table 객체 안에 unique_ptr 이용하여 array 포함 -> delete 에서 신경쓸 필요 없음  */


/*if use check() for memory_table and hash_table object  it shows state of memory
    해쉬나 메모리의 상태를 보고 싶다면 
    chect()를 써주세요

*/


int main()
{
    //size를 매개변수로 주면 변경 가능합니다
    MyHashTable hash_table(101);
    MemoryTable memory_table(101);
    MemoryTable free_list(memory_table.GetSize());
    SMsystem sm(hash_table, memory_table, free_list);


    Initalize(sm);
  
    while (true) {
        string ref;
        
        getline(cin, ref);
       
        string tmp = ref;
        ref=SMpreProcessing(ref);
        
        StrBuf buf(ref);
        

     
        sm.SetDefineFlag(false);
       

        int root_num = READ(buf, hash_table.GetSize(), sm);
        
        cout << "start____________________________________________" << endl;
        cout << "Free list's Root Node: " << free_list[0].right << endl;
        cout << "Parse tree's Root Node: " << root_num << endl;
        cout << "\n\n";
        
        /*메모리와 해쉬테이블을 보이고 싶지 않으면 이 커맨드들을 주석처리 해주세요 */
        memory_table.check();
        hash_table.check();
        /*      */


        cout << "Print first input_____" << endl;
        cout << tmp << endl;


        if (sm.GetGarbageFlag() == true) {
            sm.SetGarbageFlag(false);

            cout << "we did garbage collecting at this line , so type again please" << endl;
            continue;
        }


   


        cout << "Print input_____" << endl;
        PRINT(root_num, true, sm);
        cout << "-------------------------" << endl;
      
       

        int result;
        result = SMeval(root_num, sm);
        

        cout << "Print result_____" << endl;
        PRINT(result, true, sm);
        cout << endl;

       //Returnmemory(root_num,memory_table,free_list);
       // free_list.check();
        //free list check 가능 함수

        cout << "end_____________________________________" << "\n\n\n";
      
        //memory_table.check();

      
    }
    
    //parse 에서 free 도 돌려주기 
    
  

};
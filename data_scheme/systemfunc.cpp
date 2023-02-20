#include "systemfunc.h"
#include <cassert>
static int LEFT_PAR;
static int RIGHT_PAR;
static int PLUS;
static int MINUS;
static int TIMES;
static int isNUMBER;
static int isSYMBOL;
static int FALSE;
static int TRUE;
static int isNULL;
static int CONS;
static int COND;
static int CAR;
static int CDR;
static int DEFINE;
static int QUOTE;
static int LAMBDA;
static int ELSE;
static int SAME;
static int EQ;
static int EQUAL;

static SMStack st(16);
/*SMstack 은 SMeval 에서  정의된 펑션을 쓸때 변수들을 변경하고 저장하는 데 사용하는 
스택으로 간단하게 
{idx, val} 를 저장하고 뽑을 수 있습니다. idx: 해쉬 위치 val: 밸류
default size는 16이고 push를 할 때 사이즈를 체크하여 가득 찼으면 2배 늘려줍니다.
*/

bool CheckStructure(int idx_1, int idx_2, SMsystem& sm);



/*isStrNum function은 string을 받아서
int로 parse 될 수 있으면 true
불가능하면 false 를 리턴한다.
즉 string이 숫자인지 판단해주는 predicate
*/
bool isStrNum(string chk_str) {
	int len = chk_str.length();
	char c;
	for (int i = 0; i < len; i++) {
		c = chk_str[i];
		if (c < 48 || c>57) {
			return false;
		}
	}
	return true;
}

/*strbuf is similar to stringstream*/
StrBuf::StrBuf(const string& str)
	:mStr(str)
{}

char StrBuf::get() {
	if (mStr == "") {
		return '\0';
	}
	char c = mStr[0];
	mStr = mStr.substr(1, mStr.length() - 1);

	return c;
}
void StrBuf::putback(char c) {
	mStr = (c + mStr);
}


/*SMpreProcessing function 
string command를 주어주면 전처리를 하여서 
전처리된 string 을 return한다. \
전처리 기능 1.'(a b c)=> (quote (a b c))
			2. (define (func x) ~~)=> (define func (lambda (x) ~~~)) 로 전처리 

			GetNexttoken 을 이용해서 토큰을 읽어가면 서 
			만약에 
			'나 define 을 만나면 
			추가적으로 필요한 문구들을 ret 에 추가해주고 
			재귀적으로 ret에 계속해서 읽어준다.
*/
string SMpreProcessing(string newCommand) {
	StrBuf newBuf(newCommand);
	string ret = "";
	string token="";
	while ((token = GetNextToken(newBuf)) != "") {

		if (token == "define") {
			ret = ret + " define";
			token = GetNextToken(newBuf);
			if (token == "(") {
				token = GetNextToken(newBuf);
				
				ret = ret + " " + token + " (lambda(" + SMpreProcessing(newBuf.GetString()) + " )";
				return ret;
			}
			else {
				ret = ret + " " + token + SMpreProcessing(newBuf.GetString());
			}
		}
		else if (token == "'") {
			ret = ret + " (quote";
			int number_of_left_paren = 0;
			do
			{
				token = GetNextToken(newBuf);
				ret = ret +" "+ token;
				if (token == "(") {
					number_of_left_paren++;
				}
				else if (token == ")") {
					number_of_left_paren--;
				}

			} while (number_of_left_paren>0);
			ret = ret + ")";
		}
		else {
			ret = ret +" "+ token;
		}
	}
	
	return ret;
}




/*at initialization firstly 
insert built-in function 
and i used 
wrapper class sm which have 
hash, memo, freelist reference

*/
void Initalize(SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;
	MemoryTable& freelist = sm.freelist;

	LEFT_PAR=GetHash("(", sm);
	RIGHT_PAR=GetHash(")", sm);
	TRUE=GetHash("#t", sm);
	FALSE=GetHash("#f", sm);
	PLUS=GetHash("+", sm);
	MINUS=GetHash("-", sm);
	TIMES=GetHash("*", sm);
	isNUMBER=GetHash("number?", sm);
	isSYMBOL=GetHash("symbol?", sm);
	isNULL=GetHash("null?", sm);
	CONS=GetHash("cons", sm);
	COND=GetHash("cond", sm);
	CAR=GetHash("car", sm);
	CDR=GetHash("cdr", sm);
	DEFINE=GetHash("define", sm);
	QUOTE=GetHash("quote", sm);
	LAMBDA = GetHash("lambda", sm);
	ELSE = GetHash("else", sm);
	SAME = GetHash("=", sm);
	EQ = GetHash("eq?", sm);
	EQUAL = GetHash("equal?", sm);
	/*built in function insert*/


	
	for (int i = 1; i < memo.GetSize()-1; i++) {
		freelist[i].right = i + 1;
	}
	freelist[memo.GetSize() - 1].right = 0;
	freelist[0].right = 1;
}


/**/

/*getnexttoken function은 
strbuf 를 받아서 nexttoken을 처리해주고 strbuf 안에 있는 string 을 변화시켜준다.
*/
string GetNextToken(StrBuf& is) {
	
	if (is.GetString() == "") {
		return "";
	}
	char tmp_str[MAX_LEN];
	int len=1;
	char first_char ;
	while ((first_char = is.get()) == ' '||first_char=='\n') {
	}
	
	if (first_char == '(') {
		
		return "(";
	}
	else if (first_char == ')') {
		
		return ")";
	}
	else if (first_char == '\'') {
		return "'";
	}
	else {
		if (65 <= first_char && first_char <= 90) {
			first_char += 32;
		}
		tmp_str[0] = first_char;
		while (true) {
			first_char = is.get();
			if (first_char == '(' || first_char == ')' || first_char == ' ' || first_char == '\n' || first_char == '\0'||first_char=='\'') {
				is.putback(first_char);
				break;
			}
			else {
				if (65<=first_char&&first_char<=90) {
					first_char += 32;
				}
				tmp_str[len] = first_char;
				len++;
			}
		}
		tmp_str[len] = '\0';
		int num_len=len;
		if (48 <= tmp_str[0] && tmp_str[0] <= 57) {
			for (int i = 1; i < len; i++) {
				if (tmp_str[i] < 48 || tmp_str[i]>57) {
					num_len = i;
					break;
				}
			}
			tmp_str[num_len] = '\0';
			string ref(tmp_str);
			int num = stoi(ref);
			return to_string(num);
		}
		string ref(tmp_str);
		return ref;
	}
	
};

/*SmisNum function은 idx 를 받아서 그곳에 위치한 데이터가 숫자스트링인지 아닌지를 판단해주는 predicate 함수
idx가 양수라면 그것은 memo 상
0이라면 nil 
음수라면 hash를 의미한다.
음수일 경우만 체크를 해주고 먼저 
getval()이 마이너스가 아닐떄까지 이동해준다 .
이 만약에 getval이 양수다 => 그것은 list라는 것을 의미 false;
만약에 0일 경우에는 string 을 체크해서 number인지 판단해준다.
*/
bool SMisNum(const int idx, SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	if (idx >= 0) { return false; }
	
	int cur_idx = idx;
	while (hash[-cur_idx].GetVal() < 0) {
		cur_idx = hash[-cur_idx].GetVal();
	}
	if (hash[-cur_idx].GetVal() != HASH_NIL) {
		return false;
	}
	string chk = hash[-cur_idx].GetString();
	return isStrNum(chk);
}

/*hash에서 idx 에 맞는 val 찾아주기 idx is negative!!! because it is hash
SMgetVAl function은 해쉬상에서 idx를 받으면 그에 맞는 value 를 return 해준다.
이때 value가 int 라는 것을 가정하고 이함수를 호출!!!!!!

즉 hash 상에서 int로 define 되있거나 int인 데이터를 찾아준다. 

재귀적으로 Getval 이 0일 떄까지 돌리다가 stoi를 통해서 int를 returnb
*/
int SMgetVal(const int idx, SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	
	int cur_idx = idx;
	assert(cur_idx < 0);
	if (cur_idx >= 0) {
		printf("error: it is not number\n ");
		exit(1);
	}
	while (hash[-cur_idx].GetVal() != HASH_NIL) {
		cur_idx = hash[-cur_idx].GetVal();
		assert(cur_idx < 0);
		if (cur_idx >= 0) {
			printf("error: it is not number\n ");
			exit(1);
		}
	}
	string tmp = hash[-cur_idx].GetString();
	for (int i = 0; i < tmp.length(); i++) {
		if (isdigit(tmp.at(i)) == false) {
			cout << "error: it is not number!!" << endl;
			exit(-1);
		}
	}
	int ret = stoi(tmp);
	return ret;
}

/*헤더파일 참조*/
int GetHash(const string& str, SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	int hashnum = HashFunc_(str);
	int capa = hash.GetSize();
	hashnum = hashnum % capa;

	
	if (hashnum == 0) {
		hashnum = 1;
	}


	for (int i = hashnum; i < capa; i++) {

		if (hash[i].GetString() == str) {

			return -i;
		}

		if (hash[i].GetString() == "") {
			hash[i] = HashNode(str, HASH_NIL);
			return -i;
		}
		
	}

	for (int i = 1; i < hashnum; i++) {
		if (hash[i].GetString() == str) {

			return -i;
		}

		if (hash[i].GetString() == "") {
			hash[i] = HashNode(str, HASH_NIL);
			return -i;
		}
		
	}

	return 0;
}
/*SMisFunc는 
idx 를 주었을 때 해쉬상에서 value를 받아서 
이것이 펑션인지 판단해주는 함수
list 상에서 LAMBDA가 제위치에 있는지 체크를 해주면 된다.

*/
static bool SMisFunc(int idx, SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;
	int val = hash[-idx].GetVal();
	if (val <= 0||val==HASH_NIL) {
		return false;
	}
	if (memo[val].left == LAMBDA) {
		return true;
	}
	else {
		return false;
	}

}
/*
SMeval func 는 root_idx를 주면 

evaluation 을 해주는 함수
1. 만약에 root_idx가 음수(hash상 데이터) 이거나 NIL일 경우에 
hash상에서 value 가 음수가 아닐 때까지 재귀적으로 이동하다가 
0일 경우에는 숫자인치 체크해서 숫자이면 hash_idx를 리턴해주고 아니면 오류 

양수인 경우 그 리스트를 리턴


2. +, - ,* 사칙연산
먼저 올바른 입력인지 본다  (+ 하나 둘) 이런 꼴인지 체크 딱 2개여야 한다.

그리고 하나와 둘을 smeval 로 evaluation 해주고 smgetval()로 int로 반환하여준다. 이 때 만약 숫자가 아니라면
smgetval 에서 오류가 날 것이다. 그 이후에 사칙연산을 해주고 그 결과값을 hash에 저장해주며 그 idx를 return 해준다.

3.predicate 
: isNUmber(number?), isSysbol(symbol?) isNull(null?) 
isNumber의 경우에 입력이 (number? 하나) 꼴인지를 체크를 해준다.
그리고 그 하나를 eval 한 후에 그것이 number인지를 smisnum으로 체크

symbol? 의 경우에는 비슷하지만 
숫자가 아닌지를 체크 
경고!!!!!!!!!
(symbol? a) 는 오류를 내야한다. 
(define a 3) 
(symbol? a) => false는 ok

(number? ) 도 마찬가지


isNIL(null?) 일 경우에

먼저 입력이 (null? 하나) 꼴인지 체크를 해준다.
그리고
그 하나를 eval 한 후 그것이 NIL인지를 체크해준다.



4 cons, car, cdr 

cons 의 경우에는 2개의 매개 변수 car, cdr 은 1개의 매개변수를 받는데
이를 체크해주고 

CONS의 경우 새로운 메모리를 할당해주고  
(CONS 하나 둘) 로 주어질 경우에 
새로 할당된 메모리의 왼쪽에는 eval(하나) 오른쪽에는 eval(둘) 을 붙여준다.


(CAR 하나) 의 경우에는 eval(하나) 를 한것의 왼쪽을 
(CDR 하나) 의 경우에는 eval(하나) 를 한것의 오른쪽을 return 해준다. 

5. 
COND 의 경우 
	root
cond    		 root1
	메모리
pred1		val1		root2
			메모리
		pred2		val2		root3
					메모리
				else		vla3	NIL

이런식으로 되어있기 떄문에  root 를 이동시켜가면서
pred를 val 하고 true 가나오면 return 을 하고
else 까지 오면 else 인지를 판단한다음에 else 에 해당하는 value를 return 해준다.


6.DEFINE의 경우 case가 2개가 있따.
함수일 경우와 함수가 아닌 경우가 있는데
root 의 오른쪽의 오른쪽의 왼쪽의 왼쪽이 LAMBDA인지 판단해보면 알 수 있다.
만약 함수라면  labda가 포함된 리스트를 해쉬상에서 변수로 할당해주고
만약에 함수가 아니라면
eval 을 한값을 hash의 value로 할당해준다.

7.QUOTE
quote 의 경우 간단하게 right의 left를 return eval해주지 말것!!!!!
quote 의 의미를 생각해보자

8.
마지막으로 이미 정의된 함수의 경우

함수일 경우와 함수가 아닌 경우가 있는데
root 의 오른쪽의 오른쪽의 왼쪽의 왼쪽이 LAMBDA인지 판단해보면 알 수 있다.
만약 함수라면  labda가 포함된 리스트를 해쉬상에서 변수로 할당해주고
만약에 함수가 아니라면
eval 을 한값을 hash의 value로 할당해준다.

먼저 SMisFUNC로 token_idx로 주어진 list로 가봐서 진짜 함수인지 체크를 한다 이것은
적절한 위치에 LAMBDA가 진짜로 있는지 체크
그 이후에 변수들을 list 에 주어진 값으로 할당을 해줘야 한다.
예를 들어서 (define (append l r ) ~~~)
의 경우 hash의 l, r 로 찾아가서 변경을 해주는데
그 전에 previous value를 저장을 해놔야하기 떄문에 stack을 이용한다.
stack 에 전의 value 를 저장해 놓고  memo 상의 value를 hash 로 옮겨준다.
그리고
함수 본체로 이동 (람다 오른쪽) 해서 eval 을 진행 해준 값을 return 해주고 다시 prev값으로 hash value를 return 해준다.

9.여기 까지 통과됬으면 문제가 있는거다 정의되지 않은 함수라거나 ((1 2 3) 4 5) 이런식으로 잘못된 입력일 것 => exit을 해준다. 


과제6 추가사항!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!

1. (= number1 number2) 
이 경우 hash에 SAME으로 저장 
먼저 (= a b) 꼴인지 체크를 해주고 

number1 number2 를 eval 해준다. 그 다음에 smgetval 을 해주어 숫자를 받아오고 
==이면 #t 아니면 #f를 반환한다. 
이전에 사칙 연산을 하는 것과 상당히 유사하다.
SMgeteval 에서 주어진 idx 가 number 가 아닐 경우 error 를 내도록 설정되어있다. 


2. (eq? a  b)
이 경우 hash에 EQ로 저장 
먼저 (eq? a b) 꼴 인지 체크를 해주고 

a,b 를 eval을 해준다. 그다음에 eq 함수의 경우에는 간단하게 같은 메모리 주소이거나 해쉬주소인지 판단하면 되므로 
eval 로 주어진 값을 비교하면 끝!



3. (equal? a b)
이 경우에는 좀 복잡한다. 
a 를 eval 한 값과 b를 eval한 값이 
같은 structure 를 가지는지 체크해줘야 한다.

bool CheckStructure(int idx_1, int idx_2, SMsystem& sm)
함수를 통해서 판단을 하는데 

먼저 
매개변수로 eval 한 값을 넣어주고 
둘다 hash 안일 경우에는 
idx_1==idx_2를 return 하고 
하나만 해쉬 하나는 메모리인 경우는 false 를 리턴해준다.
만약에 메모리 값이라면

왼쪽과 오른쪽 차일드가 다 똑같은 것인지 재귀적으로 체크를 해준다.


*/

int SMeval(int root_idx, SMsystem& sm) {
	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;
	/*root_idx<0 means it is symbol in hashtable*/
	
	int root = root_idx;
	if (root < 0) {
		while (hash[-root].GetVal() < 0) {
			root = hash[-root].GetVal();
		}
		if (hash[-root].GetVal() == HASH_NIL) {
			if (SMisNum(root, sm)) {
				return root;
			}
			else {
				cout << "it is no assigned variable" << endl;
				exit(1);
				return NIL;
			}
		}
		else {
			return hash[-root].GetVal();
		}
	}
	if (root == 0) {
		return NIL;
	}

	int token_idx = memo[root_idx].left;
	if (token_idx == NIL) {
		return NIL;
	}
	if (token_idx == PLUS) {
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout<<"error at PLUS\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left==NIL) {
			cout << "error at PLUS\n";
			exit(-1);
		}

		int tmp = SMgetVal(SMeval(memo[memo[root_idx].right].left,sm),sm)+ SMgetVal(SMeval(memo[memo[memo[root_idx].right].right].left,sm),sm);
		return GetHash(to_string(tmp), sm);
	}
	else if (token_idx == MINUS) {
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout<<"error at MINUS\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left==NIL) {
			cout << "error at MINUS\n";
			exit(-1);
		}
		int tmp = SMgetVal(SMeval(memo[memo[root_idx].right].left,sm), sm) - SMgetVal(SMeval(memo[memo[memo[root_idx].right].right].left, sm), sm);
		return GetHash(to_string(tmp), sm);
	}
	else if (token_idx == TIMES) {
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout<<"error at TIMES\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left==NIL) {
			cout << "error at TIMES\n";
			exit(-1);
		}
		int tmp = SMgetVal(SMeval(memo[memo[root_idx].right].left, sm), sm) * SMgetVal(SMeval(memo[memo[memo[root_idx].right].right].left, sm), sm);
		return GetHash(to_string(tmp), sm);
	}
	else if (token_idx == SAME) {
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout << "error at =\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left == NIL) {
			cout << "error at =\n";
			exit(-1);
		}

		bool tmp= SMgetVal(SMeval(memo[memo[root_idx].right].left, sm), sm) == SMgetVal(SMeval(memo[memo[memo[root_idx].right].right].left, sm), sm);
		if (tmp) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else if (token_idx == EQ) {
		if (memo[root_idx].right == NIL) {
			cout << "error at EQ\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].right == NIL) {
			cout << "error at EQ\n";
			exit(-1);
		}
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout << "error at EQ\n";
			exit(-1);
		}

		if (SMeval(memo[memo[root_idx].right].left, sm) == SMeval(memo[memo[memo[root_idx].right].right].left, sm)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else if (token_idx == EQUAL) {
		if (memo[root_idx].right == NIL) {
			cout << "error at EQUAL\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].right == NIL) {
			cout << "error at EQUAL\n";
			exit(-1);
		}
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout << "error at EQUAL\n";
			exit(-1);
		}
		if (CheckStructure(SMeval(memo[memo[root_idx].right].left, sm), SMeval(memo[memo[memo[root_idx].right].right].left, sm),sm)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	else if (token_idx == isNUMBER) {
		if (memo[memo[root_idx].right].right != NIL) {
			cout << "error at isNUMBER\n";
			exit(-1);
		}
		if (memo[root_idx].right == NIL) {
			cout << "error at isNUMBER\n";
			exit(-1);
		}

		if (SMisNum(SMeval(memo[memo[root_idx].right].left, sm), sm)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else if (token_idx == isSYMBOL) {
		if (memo[memo[root_idx].right].right != NIL) {
			cout << "error at isSYMBOL\n";
			exit(-1);
		}
		if (memo[root_idx].right == NIL) {
			cout << "error at isSYMBOL\n";
			exit(-1);
		}

		int result = SMeval(memo[memo[root_idx].right].left,sm);
		if (result && SMisNum(result, sm) == false) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else if (token_idx == isNULL) {
		if (memo[memo[root_idx].right].right != NIL) {
			cout << "error at isNULL\n";
			exit(-1);
		}
		if (memo[root_idx].right == NIL) {
			cout << "error at isNULL\n";
			exit(-1);
		}

		if (memo[root_idx].right == NIL || SMeval(memo[memo[root_idx].right].left, sm) == NIL) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	else if (token_idx == CONS) {
		if (memo[memo[memo[root_idx].right].right].right != NIL) {
			cout << "error at CONS\n";
			exit(-1);
		}
		if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left == NIL) {
			cout << "error at CONS\n";
			exit(-1);
		}

		int newmemory = Alloc(sm);
		memo[newmemory].left = SMeval(memo[memo[root_idx].right].left,sm);
		memo[newmemory].right = SMeval(memo[memo[memo[root_idx].right].right].left,sm);
		
		return newmemory;
	}
	else if (token_idx == COND) {
		while (memo[memo[root].right].right != NIL) {
			root = memo[root].right;
			if (SMeval(memo[memo[root].left].left, sm) == TRUE) {
				return SMeval(memo[memo[memo[root].left].right].left,sm);
			}
		}
		if (memo[memo[memo[root].right].left].left != ELSE) {
			cout << "there is no else in COND ERROR\n";
			exit(-1);
		}
		else {
			return SMeval(memo[memo[memo[memo[root].right].left].right].left,sm);
		}
	}

	else if (token_idx == CAR) {
	if (memo[memo[root_idx].right].right != NIL) {
		cout << "error at CAR\n";
		exit(-1);
	}
	if (memo[root_idx].right == NIL) {
		cout << "error at CAR\n";
		exit(-1);
	}
	if (SMeval(memo[memo[root_idx].right].left, sm) <= 0) {
		cout << "error at CAR\n";
		exit(-1);
	}
		return memo[SMeval(memo[memo[root_idx].right].left, sm)].left;
	}
	else if (token_idx == CDR) {
	if (memo[memo[root_idx].right].right != NIL) {
		cout << "error at CDR\n";
		exit(-1);
	}
	if (memo[root_idx].right == NIL) {
		cout << "error at CDR\n";
		exit(-1);
	}
	if (SMeval(memo[memo[root_idx].right].left, sm) <= 0) {
		cout << "error at CDR\n";
		exit(-1);
	}

		return memo[SMeval(memo[memo[root_idx].right].left, sm)].right;
	}
	else if (token_idx == DEFINE) {
	if (memo[memo[memo[root_idx].right].right].right != NIL) {
		cout << "error at DEFINE\n";
		exit(-1);
	}
	if (memo[memo[root_idx].right].left == NIL || memo[memo[memo[root_idx].right].right].left == NIL) {
		cout << "error at DEFINE\n";
		exit(-1);
	}
		
		int tmp_loc = memo[memo[memo[root_idx].right].right].left;
		if (tmp_loc > 0 && memo[tmp_loc].left == LAMBDA) {
			hash[-memo[memo[root_idx].right].left].SetVal(memo[memo[memo[root_idx].right].right].left);
				sm.ChangeState(memo[memo[memo[root_idx].right].right].left, CONST);
			return define_print;
		}
		else {
			hash[-memo[memo[root_idx].right].left].SetVal(SMeval(memo[memo[memo[root_idx].right].right].left,sm));
			if (SMeval(memo[memo[memo[root_idx].right].right].left, sm) > 0) {
				sm.ChangeState(SMeval(memo[memo[memo[root_idx].right].right].left, sm), CONST);
			}
			return define_print;
		}
	}
	else if (token_idx == QUOTE) {
		return memo[memo[root_idx].right].left;
	}

	else if(SMisFunc(token_idx,sm)) {
		
	
		 root = memo[root].right;
		int func_cur = memo[memo[hash[-token_idx].GetVal()].right].left;
		int cnt = 0;
		while (root != NIL) {
			cnt++;
			string tmp_str = hash[-memo[func_cur].left].GetString();
			int tmp_int= hash[-memo[func_cur].left].GetVal();
			st.push({ memo[func_cur].left,tmp_int });
			int new_int = SMeval(memo[root].left,sm);
			hash[-memo[func_cur].left].SetVal(new_int);
			
			func_cur = memo[func_cur].right;
			root = memo[root].right;
		}
		
		int func_lst = hash[-token_idx].GetVal();
		int result = SMeval(memo[memo[memo[func_lst].right].right].left,sm);
		

		for (int i = 0; i < cnt;i++) {
			Pair tmp = st.pop();
			hash[-tmp.idx].SetVal(tmp.val);
		}
		return result;
	}
	/*if (token_idx<=0) {
		cout << "\\\\\\\\\\\\\\\\\\" << endl;
		cout << root_idx << endl;
		cout << token_idx << endl;
		cout << "||||||||||||||||||||||||" << endl;
		return SMeval(token_idx, sm);
	}*/
	cout << "what are you saying??" << endl;
	exit(-1);
	
}
bool CheckStructure(int idx_1, int idx_2, SMsystem& sm) {
	
	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;

	if (idx_1 < 0 && idx_2 >= 0) {
		return false;
	}
	else if (idx_1 >= 0 && idx_2 < 0) {
		return false;
	}
	else if (idx_1 < 0 && idx_2 < 0) {
		return idx_1 == idx_2;
	}
	else if(idx_1==0&&idx_2==0)
	{
		return true;
	}
	else {
		return CheckStructure(memo[idx_1].left, memo[idx_2].left, sm) && CheckStructure(memo[idx_1].right, memo[idx_2].right, sm);
	}




}



/*헤더파일 참조*/
int Alloc(SMsystem& sm) {
	MemoryTable& freelist = sm.freelist;


	int root_num = freelist[0].right;
	if (root_num == 0&&sm.GetDefineFlag() == false) {
		sm.GarbageCollect();
		sm.SetGarbageFlag(true);
		root_num = freelist[0].right;
	}

	freelist[0].right = freelist[root_num].right;
	sm.ChangeState(root_num, VARY);
	sm.AddUseCnt();

	return root_num;
}


/*Read file is */

/*헤더파일 참조*/
int READ(StrBuf& is, int size_hash, SMsystem& sm) {
	int root = NIL;
	int tmp = NIL;
	bool first = true;

	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;
	MemoryTable& freelist = sm.freelist;
	

	
	int token_hash_value = GetHash(GetNextToken(is),sm);
	/// <summary>
	/// //GetHash 는 자동으로 DIV 해줘서 idx 넣어준다 
	/// </summary>
	if (token_hash_value == DEFINE) {
		
		
	}

	if (token_hash_value == LEFT_PAR) {
		
		while ((token_hash_value = GetHash(GetNextToken(is), sm)) != RIGHT_PAR) {
			if (token_hash_value == DEFINE) {
				sm.SetDefineFlag(true);
			}

			if (first) {
				tmp = Alloc(sm);
				root = tmp;
				first = false;
			}
			else {
				memo[tmp].right = Alloc(sm);
				tmp = memo[tmp].right;
			}
			if (token_hash_value == LEFT_PAR) {
				is.putback('(');
				memo[tmp].left = READ(is, size_hash, sm);
			}
			else {
				memo[tmp].left = token_hash_value;
			}
			if (!first) {
				memo[tmp].right = NIL;
			}
		}
		return root;

	}
	else {
		return token_hash_value;
	}

}

/*idx is root when start*/
void PRINT(int idx, bool start, SMsystem& sm) {
	
	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;

	if (idx == define_print) {
		return;
	}

	if (idx == NIL) {
		cout << "()";
	}
	else if (idx < 0) {
		cout << hash[-1 * idx].GetString()<<" ";
	}
	else {
		if (start) {
			cout << "(";
		}
		PRINT(memo[idx].left, true, sm);
		if (memo[idx].right != NIL) {
			PRINT(memo[idx].right, false, sm);
		}
		else { cout << ")"; }
	}

}
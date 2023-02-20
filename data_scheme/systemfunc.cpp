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
/*SMstack �� SMeval ����  ���ǵ� ����� ���� �������� �����ϰ� �����ϴ� �� ����ϴ� 
�������� �����ϰ� 
{idx, val} �� �����ϰ� ���� �� �ֽ��ϴ�. idx: �ؽ� ��ġ val: ���
default size�� 16�̰� push�� �� �� ����� üũ�Ͽ� ���� á���� 2�� �÷��ݴϴ�.
*/

bool CheckStructure(int idx_1, int idx_2, SMsystem& sm);



/*isStrNum function�� string�� �޾Ƽ�
int�� parse �� �� ������ true
�Ұ����ϸ� false �� �����Ѵ�.
�� string�� �������� �Ǵ����ִ� predicate
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
string command�� �־��ָ� ��ó���� �Ͽ��� 
��ó���� string �� return�Ѵ�. \
��ó�� ��� 1.'(a b c)=> (quote (a b c))
			2. (define (func x) ~~)=> (define func (lambda (x) ~~~)) �� ��ó�� 

			GetNexttoken �� �̿��ؼ� ��ū�� �о�� �� 
			���࿡ 
			'�� define �� ������ 
			�߰������� �ʿ��� �������� ret �� �߰����ְ� 
			��������� ret�� ����ؼ� �о��ش�.
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

/*getnexttoken function�� 
strbuf �� �޾Ƽ� nexttoken�� ó�����ְ� strbuf �ȿ� �ִ� string �� ��ȭ�����ش�.
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

/*SmisNum function�� idx �� �޾Ƽ� �װ��� ��ġ�� �����Ͱ� ���ڽ�Ʈ������ �ƴ����� �Ǵ����ִ� predicate �Լ�
idx�� ������ �װ��� memo ��
0�̶�� nil 
������� hash�� �ǹ��Ѵ�.
������ ��츸 üũ�� ���ְ� ���� 
getval()�� ���̳ʽ��� �ƴҋ����� �̵����ش� .
�� ���࿡ getval�� ����� => �װ��� list��� ���� �ǹ� false;
���࿡ 0�� ��쿡�� string �� üũ�ؼ� number���� �Ǵ����ش�.
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

/*hash���� idx �� �´� val ã���ֱ� idx is negative!!! because it is hash
SMgetVAl function�� �ؽ��󿡼� idx�� ������ �׿� �´� value �� return ���ش�.
�̶� value�� int ��� ���� �����ϰ� ���Լ��� ȣ��!!!!!!

�� hash �󿡼� int�� define ���ְų� int�� �����͸� ã���ش�. 

��������� Getval �� 0�� ������ �����ٰ� stoi�� ���ؼ� int�� returnb
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

/*������� ����*/
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
/*SMisFunc�� 
idx �� �־��� �� �ؽ��󿡼� value�� �޾Ƽ� 
�̰��� ������� �Ǵ����ִ� �Լ�
list �󿡼� LAMBDA�� ����ġ�� �ִ��� üũ�� ���ָ� �ȴ�.

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
SMeval func �� root_idx�� �ָ� 

evaluation �� ���ִ� �Լ�
1. ���࿡ root_idx�� ����(hash�� ������) �̰ų� NIL�� ��쿡 
hash�󿡼� value �� ������ �ƴ� ������ ��������� �̵��ϴٰ� 
0�� ��쿡�� ������ġ üũ�ؼ� �����̸� hash_idx�� �������ְ� �ƴϸ� ���� 

����� ��� �� ����Ʈ�� ����


2. +, - ,* ��Ģ����
���� �ùٸ� �Է����� ����  (+ �ϳ� ��) �̷� ������ üũ �� 2������ �Ѵ�.

�׸��� �ϳ��� ���� smeval �� evaluation ���ְ� smgetval()�� int�� ��ȯ�Ͽ��ش�. �� �� ���� ���ڰ� �ƴ϶��
smgetval ���� ������ �� ���̴�. �� ���Ŀ� ��Ģ������ ���ְ� �� ������� hash�� �������ָ� �� idx�� return ���ش�.

3.predicate 
: isNUmber(number?), isSysbol(symbol?) isNull(null?) 
isNumber�� ��쿡 �Է��� (number? �ϳ�) �������� üũ�� ���ش�.
�׸��� �� �ϳ��� eval �� �Ŀ� �װ��� number������ smisnum���� üũ

symbol? �� ��쿡�� ��������� 
���ڰ� �ƴ����� üũ 
���!!!!!!!!!
(symbol? a) �� ������ �����Ѵ�. 
(define a 3) 
(symbol? a) => false�� ok

(number? ) �� ��������


isNIL(null?) �� ��쿡

���� �Է��� (null? �ϳ�) ������ üũ�� ���ش�.
�׸���
�� �ϳ��� eval �� �� �װ��� NIL������ üũ���ش�.



4 cons, car, cdr 

cons �� ��쿡�� 2���� �Ű� ���� car, cdr �� 1���� �Ű������� �޴µ�
�̸� üũ���ְ� 

CONS�� ��� ���ο� �޸𸮸� �Ҵ����ְ�  
(CONS �ϳ� ��) �� �־��� ��쿡 
���� �Ҵ�� �޸��� ���ʿ��� eval(�ϳ�) �����ʿ��� eval(��) �� �ٿ��ش�.


(CAR �ϳ�) �� ��쿡�� eval(�ϳ�) �� �Ѱ��� ������ 
(CDR �ϳ�) �� ��쿡�� eval(�ϳ�) �� �Ѱ��� �������� return ���ش�. 

5. 
COND �� ��� 
	root
cond    		 root1
	�޸�
pred1		val1		root2
			�޸�
		pred2		val2		root3
					�޸�
				else		vla3	NIL

�̷������� �Ǿ��ֱ� ������  root �� �̵����Ѱ��鼭
pred�� val �ϰ� true �������� return �� �ϰ�
else ���� ���� else ������ �Ǵ��Ѵ����� else �� �ش��ϴ� value�� return ���ش�.


6.DEFINE�� ��� case�� 2���� �ֵ�.
�Լ��� ���� �Լ��� �ƴ� ��찡 �ִµ�
root �� �������� �������� ������ ������ LAMBDA���� �Ǵ��غ��� �� �� �ִ�.
���� �Լ����  labda�� ���Ե� ����Ʈ�� �ؽ��󿡼� ������ �Ҵ����ְ�
���࿡ �Լ��� �ƴ϶��
eval �� �Ѱ��� hash�� value�� �Ҵ����ش�.

7.QUOTE
quote �� ��� �����ϰ� right�� left�� return eval������ ����!!!!!
quote �� �ǹ̸� �����غ���

8.
���������� �̹� ���ǵ� �Լ��� ���

�Լ��� ���� �Լ��� �ƴ� ��찡 �ִµ�
root �� �������� �������� ������ ������ LAMBDA���� �Ǵ��غ��� �� �� �ִ�.
���� �Լ����  labda�� ���Ե� ����Ʈ�� �ؽ��󿡼� ������ �Ҵ����ְ�
���࿡ �Լ��� �ƴ϶��
eval �� �Ѱ��� hash�� value�� �Ҵ����ش�.

���� SMisFUNC�� token_idx�� �־��� list�� ������ ��¥ �Լ����� üũ�� �Ѵ� �̰���
������ ��ġ�� LAMBDA�� ��¥�� �ִ��� üũ
�� ���Ŀ� �������� list �� �־��� ������ �Ҵ��� ����� �Ѵ�.
���� �� (define (append l r ) ~~~)
�� ��� hash�� l, r �� ã�ư��� ������ ���ִµ�
�� ���� previous value�� ������ �س����ϱ� ������ stack�� �̿��Ѵ�.
stack �� ���� value �� ������ ����  memo ���� value�� hash �� �Ű��ش�.
�׸���
�Լ� ��ü�� �̵� (���� ������) �ؼ� eval �� ���� ���� ���� return ���ְ� �ٽ� prev������ hash value�� return ���ش�.

9.���� ���� ��������� ������ �ִ°Ŵ� ���ǵ��� ���� �Լ���ų� ((1 2 3) 4 5) �̷������� �߸��� �Է��� �� => exit�� ���ش�. 


����6 �߰�����!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!

1. (= number1 number2) 
�� ��� hash�� SAME���� ���� 
���� (= a b) ������ üũ�� ���ְ� 

number1 number2 �� eval ���ش�. �� ������ smgetval �� ���־� ���ڸ� �޾ƿ��� 
==�̸� #t �ƴϸ� #f�� ��ȯ�Ѵ�. 
������ ��Ģ ������ �ϴ� �Ͱ� ����� �����ϴ�.
SMgeteval ���� �־��� idx �� number �� �ƴ� ��� error �� ������ �����Ǿ��ִ�. 


2. (eq? a  b)
�� ��� hash�� EQ�� ���� 
���� (eq? a b) �� ���� üũ�� ���ְ� 

a,b �� eval�� ���ش�. �״����� eq �Լ��� ��쿡�� �����ϰ� ���� �޸� �ּ��̰ų� �ؽ��ּ����� �Ǵ��ϸ� �ǹǷ� 
eval �� �־��� ���� ���ϸ� ��!



3. (equal? a b)
�� ��쿡�� �� �����Ѵ�. 
a �� eval �� ���� b�� eval�� ���� 
���� structure �� �������� üũ����� �Ѵ�.

bool CheckStructure(int idx_1, int idx_2, SMsystem& sm)
�Լ��� ���ؼ� �Ǵ��� �ϴµ� 

���� 
�Ű������� eval �� ���� �־��ְ� 
�Ѵ� hash ���� ��쿡�� 
idx_1==idx_2�� return �ϰ� 
�ϳ��� �ؽ� �ϳ��� �޸��� ���� false �� �������ش�.
���࿡ �޸� ���̶��

���ʰ� ������ ���ϵ尡 �� �Ȱ��� ������ ��������� üũ�� ���ش�.


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



/*������� ����*/
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

/*������� ����*/
int READ(StrBuf& is, int size_hash, SMsystem& sm) {
	int root = NIL;
	int tmp = NIL;
	bool first = true;

	MyHashTable& hash = sm.hash;
	MemoryTable& memo = sm.memo;
	MemoryTable& freelist = sm.freelist;
	

	
	int token_hash_value = GetHash(GetNextToken(is),sm);
	/// <summary>
	/// //GetHash �� �ڵ����� DIV ���༭ idx �־��ش� 
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
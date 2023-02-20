#include "MyHashTable.h"






extern unsigned int HashFunc_(const string& s) {
	int length = (int)s.length();
	unsigned int answer = 0;
	if (length % 2 == 1) {
		answer = s.at(length - 1);
		length--;
	}
	for (int i = 0; i < length; i += 2) {
		answer += s.at(i);
		answer += ((int)s.at(i + 1)) << 8;
	}
	return answer;
};



MemoryTable::MemoryTable()
	:memorytable(make_unique<MemoryNode[]>(default_size)), mCapacity(default_size)
{}
MemoryTable::MemoryTable(int size)
	:memorytable(make_unique<MemoryNode[]>(size)), mCapacity(size)
{}
void MemoryTable::SetMemory(int idx, int left, int right) {
	memorytable[idx].left = left;
	memorytable[idx].right = right;
}
MemoryNode MemoryTable::GetMemory(int idx) {
	return memorytable[idx];
}


MemoryNode& MemoryTable::operator[](int idx) const {
	if (idx >= mCapacity) {
		idx = 0;
	}
	return memorytable[idx];
}


void MemoryTable::check() const {
	cout << "this is memory table \n\n";
	for (int i = 1; i < mCapacity; i++) {
		cout << i<<"th node" << "," << "left: " << memorytable[i].left << " right: " << memorytable[i].right << endl;
	}
	cout << "\n\n\n";
};





HashNode::HashNode() 
	:mStr(""),mValue(HASH_NIL)
{};


HashNode::HashNode(string str, int val)
	:mStr(str), mValue(val)
{};
HashNode::HashNode(const HashNode& copy)
	:mStr(copy.mStr), mValue(copy.mValue)
{};

HashNode& HashNode::operator=(const HashNode& rhs) {
	mStr = rhs.mStr;
	mValue = rhs.mValue;
	return *this;
};




MyHashTable::MyHashTable()
	:mTable(make_unique<HashNode[]>(default_size)), mCapacity(default_size)
{};
MyHashTable::MyHashTable(int size)
	:mTable(make_unique<HashNode[]>(size)), mCapacity(size)
{};



void MyHashTable::check() const {
	cout << "this is hash table \n\n";
	for (size_t i = 0; i < mCapacity; i++) {
		if (mTable[i].GetString() == "") {
			continue;
		}
		if(mTable[i].GetVal()==HASH_NIL)
		cout << i<<"th node, symbol: " << mTable[i].GetString() << "  value: NIL"  << endl;
		else
			cout << i << "th node, symbol: " << mTable[i].GetString() << "  value: " << mTable[i].GetVal() << endl;
	}
	cout << "\n\n\n";
};

HashNode& MyHashTable::operator[](int num)const {
	if (num >= mCapacity) { num = 0; }
	return mTable[num];
}





void SMsystem::GarbageCollect() {
	if (useCnt < 0.7 * memorySize) {
		return;
	}

	cout << "Oh we need to clean" << endl;

	for (int i = memorySize-1; i>0; i--) {
		if (state[i] == VARY) {
			memo[i] = { 0,0 };
			int tmp = freelist[0].right;
			freelist[0].right = i;
			freelist[i].right = tmp;
			state[i] = FREE;
			DecUseCnt();
		}
	}
	cout << "clean!!!" << endl;
	cout << "using memory: " << useCnt << " total memory: " << memorySize - 1 << endl;
}

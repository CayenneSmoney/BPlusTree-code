// B+Tree.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "BPlusTree.h"
#include<iostream>
#include<string>

//k阶b+树

int main(int argc, char *argv[]) {
	runFrame();
	return 0;
}

BPlusTree* buildBplusTree() {
	BPlusTree *temp= McTree;
	temp->LinkHead = NULL;
	temp->root = NULL;
	return temp;
}

size_t bSearch(std::vector<std::string> indexs, std::string key) {
	int l = 0;
	int r = indexs.size()-1;
	while (l <= r) {
		int mid = (l + r) / 2;
		if (indexs[mid] < key) {
			l = mid + 1;
		}
		else {
			r = mid - 1;
		}
	}
	return l;
}

void up_updata(TreeNode *p, std::vector<TreeNode*> Stack,int i,size_t pos) {
	for (; i >= 0; i--) {
		TreeNode *parent = Stack[i];
		pos = bSearch(parent->keys, p->keys[0]);
		if (parent->keys[pos] == p->keys[p->keys.size() - 1]) {
			break;
		}
		
		//parent->values.sons[pos]==p
		parent->keys[pos] = p->keys[p->keys.size() - 1];
		p = parent;
	}
}

TreeNode* findNode(BPlusTree *bTree, std::string key,std::vector<TreeNode*> &s) {
	if (bTree->LinkHead==NULL && bTree->root==NULL) {
		return NULL;
	}
	assert(bTree->LinkHead&&bTree->root);
	TreeNode* p = bTree->root;
	while (p->tag) {
		s.push_back(p);
		size_t pos = bSearch(p->keys, key);
		if (pos == p->values.sons.size()) {
			pos--;
		}
		p = p->values.sons[pos];
	}
	return p;
}

bool Find(BPlusTree *bTree, std::string key, int &data) {
	std::vector<TreeNode*> Stack;
	TreeNode* leaf = findNode(bTree, key, Stack);
	if (leaf == NULL) {
		return false;
	}
	assert(leaf&&!leaf->tag);
	size_t pos = bSearch(leaf->keys, key);
	data = leaf->values.data[pos];
	return leaf->keys[pos] == key ? true : false;
}

void rootDivide(BPlusTree *bTree, TreeNode* parent) {
	//需要新建两个节点
	TreeNode * newRoot = McNode;
	TreeNode * rRoot = McNode;
	rRoot->tag = parent->tag;
	newRoot->tag = 1;
	if (rRoot->tag == 0) {
		rRoot->rLink = parent->rLink;
		parent->rLink = rRoot;
	}
	for (int i = (parent->keys.size()) / 2; i != parent->keys.size();) {
		rRoot->keys.push_back(parent->keys[i]);
		if (rRoot->tag) {
			rRoot->values.sons.push_back(parent->values.sons[i]);
			parent->values.sons.erase(parent->values.sons.begin() + i);
		}
		else {
			
			rRoot->values.data.push_back(parent->values.data[i]);
			parent->values.data.erase(parent->values.data.begin() + i);
		}
		parent->keys.erase(parent->keys.begin() + i);
	}
	bTree->root = newRoot;
	newRoot->keys.push_back(parent->keys[parent->keys.size() - 1]);
	newRoot->values.sons.push_back(parent);
	newRoot->keys.push_back(rRoot->keys[rRoot->keys.size() - 1]);
	newRoot->values.sons.push_back(rRoot);
}

bool add(BPlusTree *bTree, std::string key, int data) {
	std::vector<TreeNode*> Stack;
	TreeNode* leaf = findNode(bTree, key,Stack);//查找叶子节点
	if (leaf == NULL) {//空树
		assert(bTree->LinkHead == NULL&&bTree->root == NULL);
		//节点
		TreeNode *TNode=McNode;
		TNode->keys.push_back(key);
		TNode->tag = 0;
		TNode->values.data.push_back(data);
		//跟新树
		bTree->LinkHead = TNode;
		TNode->rLink = NULL;
		bTree->root = TNode;
		return true;
	}
	assert(leaf->keys.size()==leaf->values.data.size());
	std::vector<std::string>::iterator it;
	if ((it=find(leaf->keys.begin(), leaf->keys.end(),key))!= leaf->keys.end()) {//找到该关键字，插入失败
		return false;
	}
	//向上修改内部节点最大值
	size_t pos = bSearch(leaf->keys, key);
	leaf->values.data.insert(leaf->values.data.begin() + pos, data);
	leaf->keys.insert(leaf->keys.begin() + pos, key);
	if (pos == leaf->keys.size() - 1&&!Stack.empty()) {
		//往上修改
		size_t pos_ = bSearch(Stack[Stack.size()-1]->keys, leaf->keys[leaf->keys.size() - 2]);
		up_updata(leaf,Stack,Stack.size()-1,pos_);

	}
	if (leaf->keys.size()<=k) {
		return true;
	}
	assert(leaf->keys.size() == k+1);//需要分裂操作
	if (Stack.empty()) {
		rootDivide(bTree, leaf);
		return true;
	}
	int i = Stack.size() - 1;
	TreeNode *parent = Stack[i--];//i是栈顶指针
	TreeNode *p = leaf;
	while (p->keys.size()>k) {//循环分裂
		//前k/2个节点做原节点   后面的新创节点保存
		TreeNode *q = McNode;
		q->tag = p->tag;
		if (p->tag == 0) {
			q->rLink = p->rLink;
			p->rLink = q;
		}
		for (int i = p->keys.size() / 2; i != p->keys.size(); ) {
			q->keys.push_back(p->keys[i]);
			if (q->tag) {
				q->values.sons.push_back(p->values.sons[i]);
				p->values.sons.erase(p->values.sons.begin() + i);
			}else {
				q->values.data.push_back(p->values.data[i]);
				p->values.data.erase(p->values.data.begin() + i);
			}
			p->keys.erase(p->keys.begin() + i);
		}
		size_t pos_p = bSearch(parent->keys, q->keys[q->keys.size() - 1]);
		parent->keys[pos_p] = p->keys[p->keys.size() - 1];
		//parent中添加一个指向q
		parent->keys.insert(parent->keys.begin() + pos_p + 1, q->keys[q->keys.size() - 1]);
		parent->values.sons.insert(parent->values.sons.begin() + pos_p + 1, q);
		if (parent==bTree->root) {//到根节点了
			if (parent->keys.size() > k) {
				rootDivide(bTree, parent);
			}
			break;
		}
		p = parent;
		parent = Stack[i--];
	}
	return true;
}


bool remove(BPlusTree *bTree, std::string key) {
	if (bTree->root == NULL) {
		return false;
	}
	std::vector<TreeNode*> Stack;
	TreeNode* leaf = findNode(bTree, key, Stack);//查找叶子节点

	assert(leaf->keys.size() == leaf->values.data.size());
	std::vector<std::string>::iterator it;
	if ((it = find(leaf->keys.begin(), leaf->keys.end(), key)) == leaf->keys.end()) {//没找到该关键字，删除失败
		return false;
	}
	//向上修改内部节点最大值
	size_t pos = bSearch(leaf->keys, key);
	assert(leaf->keys[pos] == key);
	leaf->values.data.erase(leaf->values.data.begin() + pos);
	leaf->keys.erase(leaf->keys.begin() + pos);
	if (pos == leaf->keys.size()&&!Stack.empty()) {
		//往上修改
		size_t pos_ = bSearch(Stack[Stack.size() -1]->keys, key);
		up_updata(leaf, Stack,Stack.size() -1, pos_);
	}
	if (leaf->keys.size() >=ceil(k/2.0)||leaf==bTree->root) {//直接删除
		return true;
	}

	int i = Stack.size() - 1;
	TreeNode *parent = Stack[i--];
	TreeNode *p = leaf;
	while (p->keys.size()<ceil(k/2.0)) {
		size_t pos = bSearch(parent->keys, p->keys[p->keys.size() - 1]);
		//有左兄弟，并且足够借
		TreeNode * lBro=NULL;
		if (pos > 0) {
			lBro = parent->values.sons[pos - 1];
		}
		if (lBro&&lBro->keys.size() > ceil(k / 2.0)) {
			int count = ceil((lBro->keys.size() - ceil(k / 2.0)) / 2.0);
			p->keys.insert(p->keys.begin(), lBro->keys.end() - count, lBro->keys.end());
			if (p->tag) {
				p->values.sons.insert(p->values.sons.begin(), lBro->values.sons.end() - count, lBro->values.sons.end());
				lBro->values.sons.erase(lBro->values.sons.end() - count, lBro->values.sons.end());
			}
			else {
				p->values.data.insert(p->values.data.begin(), lBro->values.data.end() - count, lBro->values.data.end());
				lBro->values.data.erase(lBro->values.data.end() - count, lBro->values.data.end());
			}
			lBro->keys.erase(lBro->keys.end() - count, lBro->keys.end());
			up_updata(lBro, Stack,i+1, pos - 1);

			break;
		}
		//有右兄弟，并且足够借
		TreeNode * rBro = NULL;
		if (pos!=parent->keys.size() -1) {
			rBro = parent->values.sons[pos + 1];
		}
		if (rBro&&rBro->keys.size() > ceil(k / 2.0)) {
			size_t Count = ceil((rBro->keys.size() - ceil(k / 2.0)) / 2.0);
			p->keys.insert(p->keys.end(), rBro->keys.begin(), rBro->keys.begin() + Count);
			rBro->keys.erase(rBro->keys.begin(), rBro->keys.begin() + Count);
			if (rBro->tag) {
				p->values.sons.insert(p->values.sons.end(), rBro->values.sons.begin(), rBro->values.sons.begin()+Count);
				rBro->values.sons.erase(rBro->values.sons.begin(), rBro->values.sons.begin() + Count);
			}
			else {
				p->values.data.insert(p->values.data.end(), rBro->values.data.begin(), rBro->values.data.begin() + Count);
				rBro->values.data.erase(rBro->values.data.begin(), rBro->values.data.begin() + Count);
			}
			up_updata(p, Stack, i + 1, pos);
			break;
		}
		//与左兄弟合并
		//p数据转移到左兄弟
		if (lBro) {
			lBro->keys.insert(lBro->keys.end(),p->keys.begin(), p->keys.end());
			if (lBro->tag) {
				lBro->values.sons.insert(lBro->values.sons.end(), p->values.sons.begin(), p->values.sons.end());
			}else {
				lBro->values.data.insert(lBro->values.data.end(), p->values.data.begin(), p->values.data.end());
			}
			parent->keys[pos - 1] = lBro->keys[lBro->keys.size() - 1];//父亲关键字调整
			parent->keys.erase(parent->keys.begin() + pos);
			parent->values.sons.erase(parent->values.sons.begin() + pos);
			if (p->tag == 0) {
				lBro->rLink = p->rLink;
			}
			delete p;
			if (parent == bTree->root&&parent->keys.size()==1) {//根节点只有一个孩子时候，删除
				bTree->root = lBro;
				delete parent;
				break;
			}
			
		}else {
		//右兄弟数据转移到p
			p->keys.insert(p->keys.end(), rBro->keys.begin(), rBro->keys.end());
			if (p->tag) {
				p->values.sons.insert(p->values.sons.end(), rBro->values.sons.begin(), rBro->values.sons.end());
			}
			else {
				p->values.data.insert(p->values.data.end(), rBro->values.data.begin(), rBro->values.data.end());
			}
			parent->keys[pos ] = p->keys[p->keys.size() - 1];
			parent->keys.erase(parent->keys.begin() + pos+1);
			parent->values.sons.erase(parent->values.sons.begin() + pos+1);
			if (p->tag == 0) {
				p->rLink = rBro->rLink;
			}
			delete rBro;
			if (parent == bTree->root&&parent->keys.size() == 1) {//根节点只有一个孩子时候，删除
				bTree->root = p;
				delete parent;
				break;
			}
		}
		p = parent;
		if (i < 0) {
			break;
		}
		parent = Stack[i--];
	}
	return true;
}

void print(TreeNode * tNode) {
	for (int i = 0; i < tNode->keys.size(); i++) {
		if (tNode->tag) {
			std::cout << tNode->keys[i] << " ";
		}
		else {
			std::cout << tNode->keys[i] << ":" << tNode->values.data[i] << " ";
		}
		
	}
	std::cout << "\t";
}

void showBTree(BPlusTree * bTree) {
	printf("******************\n");
	std::queue<TreeNode*> q;
	q.push(bTree->root);
	int printed = 0;//已经打印
	int count = 1;//当前行最后一个是第几个
	int que_count = 1;//已经入队的个数
	while (!q.empty()) {
		if (printed == count) {
			count = que_count;
		}
		printed++;
		TreeNode* Node = q.front();
		q.pop();
		print(Node);
		if (printed == count) {
			std::cout << "\n";
		}
		if (Node->tag) {
			for (int i = 0; i < Node->keys.size(); i++) {
				que_count++;
				q.push(Node->values.sons[i]);
			}
		}
	}
	printf("******************\n");
}

void runFrame() {
	BPlusTree * B = buildBplusTree();
	std::cout << "0，输入b+树的阶数k\n" << std::endl;
	std::cout << "k:";
	std::cin >> k;
	while (true) {
		std::cout << "************************\n" << std::endl;
		std::cout << "1，向b+树添加一个元素，key为字符串，data为数字\n" << std::endl;
		std::cout << "2，向b+树删除指定的key关键字\n" << std::endl;
		std::cout << "3，向b+树查找指定key对应的数据\n" << std::endl;
		std::cout << "4，展示b+树\n" << std::endl;
		std::cout << "5，退出程序\n" << std::endl;
		std::cout << "************************\n" << std::endl;
		int opt;
		int data;
		bool succ;
		std::string key;
		std::cin >> opt;
		switch (opt) {
		case 1:
			std::cout << "key:";
			std::cin >> key;
			int data;
			std::cout << "data:";
			std::cin >> data;
			succ = add(B, key, data);
			if (succ) {
				std::cout << "添加成功\n";
			}
			else {
				std::cout << "添加失败\n";
			}
			break;
		case 2:
			std::cout << "key:";
			std::cin >> key;
			succ = remove(B, key);
			if (succ) {
				std::cout << "删除成功\n";
			}
			else {
				std::cout << "删除失败\n";
			}
			break;
		case 3:

			std::cout << "key:";
			std::cin >> key;
			succ = Find(B, key, data);
			std::cout << data << "\n";
			if (succ) {
				std::cout << "查找成功\n";
			}
			else {
				std::cout << "查找失败\n";
			}
			break;
		case 4:
			showBTree(B);
			break;
		default:
			return;

		}
	}
}
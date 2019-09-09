#include"stdafx.h"
#include <stdio.h>
#include<iostream>
#include<assert.h>
#include<stack>
#include<queue>
#include<math.h>
#include<vector>
#define McTree new BPlusTree
#define McNode new TreeNode

int k;

typedef struct TreeNode{
	std::vector<std::string> keys;
	bool tag;
	//tag为0表示叶子节点
	struct {
		std::vector<int> data;
		std::vector<TreeNode*> sons;
	} values;
	TreeNode * rLink;
}TreeNode,* TreePtr;
typedef struct BPlusTree {
	TreePtr root;
	TreePtr LinkHead;
}BPlusTree;

BPlusTree* buildBplusTree();
size_t bSearch(std::vector<std::string> indexs, std::string key);
void up_updata(TreeNode *p, std::vector<TreeNode*> Stack, int i, size_t pos);
TreeNode* findNode(BPlusTree *bTree, std::string key, std::vector<TreeNode*> &s);
bool Find(BPlusTree *bTree, std::string key, int &data);
bool add(BPlusTree *bTree, std::string key, int data);
bool remove(BPlusTree *bTree, std::string key);
void rootDivide(BPlusTree *bTree, TreeNode* parent);
void showBTree(BPlusTree * bTree);
void print(TreeNode * tNode);
void runFrame();
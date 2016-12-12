#include "treenode.h"
#include "parsercpp.tab.h"
#include <iostream>

using namespace std;

TreeNode::TreeNode(){
	isBool = false;
	isArray= false;
	isRecord = false;
	isStatic = false;
	isConstant = false;
	isStatementUnderFunction = false;
	attr.name = "";
}

TreeNode::TreeNode(NodeKind nk, TokenData* tokenData){
	lineNum = tokenData->lineNum;
	nodeKind = nk;
	isBool = false;
	isArray= false;
	isRecord = false;
	isStatic = false;
	isConstant = false;
	isStatementUnderFunction = false;
	attr.name = "";
	switch(nk){
	case DECLK:
		size = 1;
		//fall through
	case OPK:
	case RECDECLK:
	case FUNDECLK:
	case PARAMK:
	case CALLK:
	case EXPK:
		attr.name = tokenData->input;
		break;
	case CONSTK:
		attr.value = tokenData->value;
		if(tokenData->tokenClass == BOOLCONST)
			isBool = true;
		isConstant = true;
		break;
	case STMTK:
		break;
	default:
		cerr << "NodeKind not defined in constructor for " << tokenData->input << endl;
  }
}

TreeNode::TreeNode(NodeKind nk, int lineNum, string name, int value = -1, bool isaBool = false){
	this->lineNum = lineNum;
	nodeKind = nk;
	isBool = false;
	isArray= false;
	isRecord = false;
	isStatic = false;
	isConstant = false;
	isStatementUnderFunction = false;
	switch(nk){
	case OPK:
	case RECDECLK:
	case FUNDECLK:
	case PARAMK:
	case CALLK:
	case DECLK:
	case EXPK:
		attr.name = name;
		break;
	case CONSTK:
		attr.value = value;
		if(isaBool)
			isBool = true;
		isConstant = true;
		break;
	case STMTK:
		break;
	default:
		cerr << "NodeKind not defined in constructor for " << name << endl;
  }
}

void TreeNode::appendSibling(TreeNode* treeNodePtr)
{
	if(this == NULL){
		return;
	}
	TreeNode* nodePtr = this;
	while(nodePtr->sibling != NULL)
	{
	  nodePtr = nodePtr->sibling;
	}
	nodePtr->sibling = treeNodePtr;
}

int TreeNode::countSiblings()
{
	int numSiblings = 0;
	TreeNode* nodePtr = this;
	while(nodePtr->sibling != NULL)
    {
		nodePtr = nodePtr->sibling;
		numSiblings++;
    }
	return numSiblings;
}

int TreeNode::totalSize(){
	//things that have size: declk
	int sum = 0;
	
	if(this == NULL){
		return 0;
	}
	
	if(nodeKind == DECLK && !isStatic){
		sum += size;
	}
	
	for(int i = 0; i < child.size(); i++){
		if(child[i] == NULL){
			continue;
		}
		sum += child[i]->totalSize();
	}
	
	TreeNode* siblingPtr = sibling;
	while (siblingPtr != NULL){
		sum += siblingPtr->totalSizeSibling();
		siblingPtr = siblingPtr->sibling;
	}
	
	return sum;
}


int TreeNode::totalSizeSibling(){
	int sum = 0;
	if(nodeKind == DECLK && !isStatic){
		sum += size;
	}
	
	for(int i = 0; i < child.size(); i++){
		if(child[i] == NULL){
			continue;
		}
		sum += child[i]->totalSize();
	}
	
	return sum;
}
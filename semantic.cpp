#include "treenode.h"
#include "parsercpp.tab.h"
#include "symtab.h"
#include <iostream>
#include <stdio.h>
#include <sstream>

using namespace std;

extern int numErrors;
extern int numWarnings;
int currLocalLocation;
int currGlobalLocation = 0;

std::map<OpKind, std::vector< std::vector<ReturnType> > > TreeNode::opMap;

void TreeNode::initializeMap(){
	if(opMap.empty()){
		opMap[ANDK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{BOOLTYPE}, vector<ReturnType>{BOOLTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[ORK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{BOOLTYPE}, vector<ReturnType>{BOOLTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[EQK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[NOTEQK] = vector< vector <ReturnType>  >
			{vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[LESSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[GRTK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[LESSEQK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[GRTEQK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{CHARTYPE, INTTYPE}, vector<ReturnType>{BOOLTYPE}};
		opMap[ARRACCK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{ARRAYALLOWED, ARRAYREQUIRED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{TYPEOFLHS}};
		opMap[ASSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE}, vector<ReturnType>{ARRAYALLOWED, TYPEOFLHS}};
		
		opMap[ADDASSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[SUBASSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[MULASSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[DIVASSK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[ADDK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[SUBK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[MULK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[DIVK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[MODK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		opMap[DOTK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}, vector<ReturnType>{INTTYPE}};
		
		//unary ops
		opMap[RANDK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{}, vector<ReturnType>{INTTYPE}};
		opMap[INCK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{}, vector<ReturnType>{INTTYPE}};
		opMap[DECK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{}, vector<ReturnType>{INTTYPE}};	
		opMap[NOTK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{BOOLTYPE}, vector<ReturnType>{}, vector<ReturnType>{BOOLTYPE}};
		opMap[ARRSIZEK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{ARRAYALLOWED, ARRAYREQUIRED, BOOLTYPE, CHARTYPE, INTTYPE, VOIDTYPE}, vector<ReturnType>{}, vector<ReturnType>{INTTYPE}};
		opMap[NEGK] = vector< vector <ReturnType>  > 
			{vector<ReturnType>{INTTYPE}, vector<ReturnType>{}, vector<ReturnType>{INTTYPE}};
	}
}

// a simple default print routine 
extern void ePrint(void *p);
SymTab TreeNode::symTab(ePrint);

void TreeNode::scopeAndType(){
	initializeMap();
	scopeAndType(0);
	if(symTab.lookup("main") == NULL){
		cout << "ERROR(LINKER): Procedure main is not defined." << endl;
		numErrors++;
	}
}


string toString(ReturnType r){
	switch(r){
	case BOOLTYPE:
		return "type bool";
		break;
	case INTTYPE:
		return "type int";
		break;
	case CHARTYPE:
		return "type char";
		break;
	case RECORDTYPE:
		return "record type";
		break;
	case VOIDTYPE:
		return "type void";
		break;
	case UNDEFINEDTYPE:
		return "type undefined";
		break;
	case ARRAYALLOWED:
	case ARRAYREQUIRED:
	case TYPEOFLHS:
		return "";
	}
}

string toString(vector<ReturnType> v){
	if(v.size() < 0)
		return "";
	
	vector<ReturnType> nv = vector<ReturnType>{ARRAYALLOWED, BOOLTYPE, CHARTYPE, INTTYPE};
	if (v.size() == 4){
		bool nonvoid = true;
		for(int i = 0; i < 4; i++){
			if (v[i] != nv[i]){
				nonvoid = false;
				break;
			}
		}
		
		if(nonvoid)
			return "NONVOID";
	}
	
	string s = toString(v[0]);
	for(int i = 1; i < v.size(); i++){
		s += " or " + toString(v[i]);
	}
	return s;
}

int numLoopsInsideOf = 0;
TreeNode* funcInsideOf = NULL;
int numReturnsInFunc;

void TreeNode::scopeAndType(int depth){
	doSymbolTableStuff();
	enterIfNecessary();
	if(nodeKind == CALLK){
		TreeNode* nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
		checkParams(nodePtr, depth);
	}
	else {
		for(int i = 0; i < child.size(); i++){
			if(child[i] == NULL){
				continue;
			}
			child[i]->scopeAndType(depth + 1);
		}
	}
	typeSelf();
	leaveIfNecessary();
	TreeNode* siblingPtr = sibling;
	while (siblingPtr != NULL){
		siblingPtr->scopeAndTypeSibling(depth);
		siblingPtr = siblingPtr->sibling;
	}
}

void TreeNode::scopeAndTypeSibling(int depth){
	doSymbolTableStuff();
	enterIfNecessary();
	if(nodeKind == CALLK){
		TreeNode* nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
		checkParams(nodePtr, depth);
	}
	else {
		for(int i = 0; i < child.size(); i++){
			if(child[i] == NULL){
				continue;
			}
			child[i]->scopeAndType(depth + 1);
		}
	}
	typeSelf();
	leaveIfNecessary();
}

void TreeNode::enterIfNecessary(){
	switch(nodeKind){
	case RECDECLK:
		symTab.enter((char *)attr.name.c_str());
		break;
	case STMTK:
		switch(kind.stmt){
		case COMPOUNDK:
			if(!isStatementUnderFunction){
				symTab.enter((char *)"Compound Statement");
			}
			break;
		case SELECTIONK:
			break;
		case ITERATIONK:
			numLoopsInsideOf++;
			break;
		case RETURNK:
			break;
		case BREAKK:
			break;
		}
		break;
	case EXPK:
		break;
	case CONSTK:
		break;
	case CALLK:
		break;
	case FUNDECLK:
		symTab.enter((char *)attr.name.c_str());
		funcInsideOf = this;
		break;
	case DECLK:
		break;
	case PARAMK:
		break;
	case OPK:
		break;
	default:
		cout << "NodeKind undefined in scopeAndTypeSelf";
	}
}

void TreeNode::leaveIfNecessary(){
	switch(nodeKind){
	case RECDECLK:
		symTab.leave();
		break;
	case STMTK:
		switch(kind.stmt){
		case COMPOUNDK:
			if(!isStatementUnderFunction){
				symTab.leave();
			}
			break;
		case ITERATIONK:
			numLoopsInsideOf--;
			break;
		}
		break;
	case FUNDECLK:
		symTab.leave();
		funcInsideOf = NULL;
		if(lineNum != -1 && numReturnsInFunc == 0 && returnType != VOIDTYPE){
			printf("WARNING(%d): Expecting to return %s but function '%s' has no return statement.\n",
				lineNum, toString(returnType).c_str(), attr.name.c_str());
			numWarnings++;
		}
		numReturnsInFunc = 0;
		break;
	}
}

bool contains(vector<ReturnType> v, ReturnType r){
	for(int i = 0; i < v.size(); i++){
		if (v[i] == r)
			return true;
	}
	return false;
}

void TreeNode::checkOp(){
	bool leftError = false, rightError = false, unexpectedArrayError = false;
	if(kind.op == ARRACCK){ //must be int on rhs 
		if(child[0] != NULL && !child[0]->isArray){
			if(child[0]->nodeKind == OPK){
				printf("ERROR(%d): Cannot index nonarray.\n", lineNum);
				numErrors++;
			}
			else{
				printf("ERROR(%d): Cannot index nonarray '%s'.\n", lineNum, child[0]->attr.name.c_str());
				numErrors++;
			}
		} 
		if(child[1] != NULL && child[1]->returnType != INTTYPE && child[1]->returnType != UNDEFINEDTYPE){
			printf("ERROR(%d): Array '%s' should be indexed by type int but got %s.\n", 
				lineNum, child[0]->attr.name.c_str(), toString(child[1]->returnType).c_str());
			numErrors++;
		}
		if(child[1] != NULL && child[1]->isArray && child[1]->returnType != UNDEFINEDTYPE){
			if(child[1]->nodeKind == OPK){
				printf("ERROR(%d): Array index is an unindexed array.\n", 
				lineNum, child[1]->attr.name.c_str());
				numErrors++;
			}
			else {
				printf("ERROR(%d): Array index is the unindexed array '%s'.\n", 
					lineNum, child[1]->attr.name.c_str());
				numErrors++;
			}
		}		
		return;
	}
	
	// Handle a unary op
	if(opMap[kind.op][1].size() == 0){
		if(child.size() > 0 && child[0] != NULL && child[0]->returnType != UNDEFINEDTYPE){
			if(!contains(opMap[kind.op][0], child[0]->returnType)){
				printf("ERROR(%d): Unary '%s' requires an operand of %s but was given %s.\n", 
					lineNum, attr.name.c_str(), toString(opMap[kind.op][0]).c_str(), toString(child[0]->returnType).c_str());
				numErrors++;
				leftError = true;
			}
			if(child[0]->isConstant){
				isConstant = true;
			}
		}
	}
	
	// Handle a binary op
	else{
		if(child.size() > 0 && child[0] != NULL && child[0]->returnType != UNDEFINEDTYPE){
			if(!contains(opMap[kind.op][0], child[0]->returnType)){
				printf("ERROR(%d): '%s' requires operands of %s but lhs is of %s.\n", 
					lineNum, attr.name.c_str(), toString(opMap[kind.op][0]).c_str(), toString(child[0]->returnType).c_str());
				numErrors++;
				leftError = true;
			}
		}

		//check that right side is valid type
		if(child.size() > 1 && child[1] != NULL && child[1]->returnType != UNDEFINEDTYPE){
			if(!contains(opMap[kind.op][1], child[1]->returnType)){
				printf("ERROR(%d): '%s' requires operands of %s but rhs is of %s.\n",
					lineNum, attr.name.c_str(), toString(opMap[kind.op][1]).c_str(), toString(child[1]->returnType).c_str());
				numErrors++;
				rightError = true;
			}
			if(!leftError && ! rightError && child[0] != NULL && child[0]->returnType != UNDEFINEDTYPE && child[0]->returnType != child[1]->returnType){
				printf("ERROR(%d): '%s' requires operands of the same type but lhs is %s and rhs is %s.\n",
					lineNum, attr.name.c_str(), toString(child[0]->returnType).c_str(), toString(child[1]->returnType).c_str());
				numErrors++;
			}
			if(!leftError && child[0] != NULL && child[0]->returnType != UNDEFINEDTYPE 
				&& contains(opMap[kind.op][0], ARRAYALLOWED) && child[0]->isArray != child[1]->isArray)
			{
				printf("ERROR(%d): '%s' requires that either both or neither operands be arrays.\n",
					lineNum, attr.name.c_str());
				numErrors++;
			}
			if(child[0]->isConstant && child[1]->isConstant){
				isConstant = true;
			}
		}
	}	
	if(child.size() > 0 && child[0] != NULL && child[0]->returnType != UNDEFINEDTYPE){
		if(!child[0]->isArray && contains(opMap[kind.op][0], ARRAYREQUIRED)){
			printf("ERROR(%d): The operation '%s' only works with arrays.\n", lineNum, attr.name.c_str());
			numErrors++;
		}
		if(child[0]->isArray && !contains(opMap[kind.op][0], ARRAYALLOWED)){
			printf("ERROR(%d): The operation '%s' does not work with arrays.\n", lineNum, attr.name.c_str());
			numErrors++;
			unexpectedArrayError = true;
		}
	}
	
	if(child.size() > 1 && child[1] != NULL && child[1]->returnType != UNDEFINEDTYPE){
			if(!child[1]->isArray && contains(opMap[kind.op][1], ARRAYREQUIRED)){
			printf("ERROR(%d): The operation '%s' only works with arrays.\n", lineNum, attr.name.c_str());
			numErrors++;
		}
		if(!unexpectedArrayError && child[1]->isArray && !contains(opMap[kind.op][1], ARRAYALLOWED)){
			printf("ERROR(%d): The operation '%s' does not work with arrays.\n", lineNum, attr.name.c_str());
			numErrors++;
		}
	}
}

void TreeNode::typeSelf(){
	TreeNode* nodePtr;
	switch(nodeKind){
	case RECDECLK:
		//type is undefined
		returnType = UNDEFINEDTYPE;
		break;
	case STMTK:
		//type is undefined
		returnType = VOIDTYPE;
		switch(kind.stmt){
		case COMPOUNDK:
			break;
		case SELECTIONK:
			if(child.size() > 0 && child[0] != NULL){
				if(child[0]->returnType != BOOLTYPE && child[0]->returnType != UNDEFINEDTYPE){
					printf("ERROR(%d): Expecting Boolean test condition in if statement but got %s.\n",
						lineNum, toString(child[0]->returnType).c_str());
					numErrors++;
				}
				if(child[0]->isArray){
					printf("ERROR(%d): Cannot use array as test condition in if statement.\n",
						lineNum);
					numErrors++;
				}
			}
			break;
		case ITERATIONK:
			if(child.size() > 0 && child[0] != NULL){
				if(child[0]->returnType != BOOLTYPE && child[0]->returnType != UNDEFINEDTYPE){
					printf("ERROR(%d): Expecting Boolean test condition in while statement but got %s.\n",
						lineNum, toString(child[0]->returnType).c_str());
					numErrors++;
				}
				if(child[0]->isArray){
					printf("ERROR(%d): Cannot use array as test condition in while statement.\n",
						lineNum);
					numErrors++;
				}
			}
			break;
		case RETURNK:
			numReturnsInFunc++;
			if(child.size() > 0 && child[0] != NULL){
				//returnType = child[0]->returnType;
				returnType = VOIDTYPE;
				if(funcInsideOf->returnType == VOIDTYPE){
					printf("ERROR(%d): Function '%s' at line %d is expecting no return value, but return has return value.\n",
						lineNum, funcInsideOf->attr.name.c_str(), funcInsideOf->lineNum);
					numErrors++;
				}
				else if(child[0]->returnType != funcInsideOf->returnType && child[0]->returnType != UNDEFINEDTYPE){
					printf("ERROR(%d): Function '%s' at line %d is expecting to return %s but instead returns %s.\n",
						lineNum, funcInsideOf->attr.name.c_str(), funcInsideOf->lineNum, toString(funcInsideOf->returnType).c_str(), 
						toString(child[0]->returnType).c_str());
					numErrors++;
				
				}
				if(child[0]->isArray){
					printf("ERROR(%d): Cannot return an array.\n", lineNum);
					numErrors++;
				}
			}
			else if(funcInsideOf->returnType != VOIDTYPE) {
				printf("ERROR(%d): Function '%s' at line %d is expecting to return %s but return has no return value.\n",
					lineNum, funcInsideOf->attr.name.c_str(), funcInsideOf->lineNum, toString(funcInsideOf->returnType).c_str());
					numErrors++;
			}
			break;
		case BREAKK:
			if(numLoopsInsideOf == 0){
				printf("ERROR(%d): Cannot have a break statement outside of loop.\n", lineNum);
				numErrors++;
			}
			break;
		}
		break;
	case EXPK:
		switch(kind.exp){
		case VARK:
			nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
			if (nodePtr == NULL){
				returnType = UNDEFINEDTYPE;
			} 
			else {
				if(nodePtr->nodeKind == FUNDECLK)
					returnType = UNDEFINEDTYPE;
				else
					returnType = nodePtr->returnType;
				isArray = nodePtr->isArray;
			}
			break;
		}
		break;
	case CONSTK:
		//type is kind.constant
		if(kind.constant == NUMC)
			returnType = INTTYPE;
		else if(kind.constant == CHARC){
			returnType = CHARTYPE;
		}
		else{
			returnType = BOOLTYPE;
		}
		break;
	case CALLK:
		//has type of its return type
		nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
		if (nodePtr == NULL){
			returnType = UNDEFINEDTYPE;
		} 
		else {
			returnType = nodePtr->returnType;
			isArray = nodePtr->isArray;
		}
		break;
	case FUNDECLK:
		//type is kind.decl
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		case VOIDK:
			returnType = VOIDTYPE;
			break;
		}
		break;
	case DECLK:
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		}
		if(child.size() > 0 && child[0] != NULL){ //check initialized value
			if(!child[0]->isConstant){
				printf("ERROR(%d): Initializer for variable '%s' is not a constant expression.\n", lineNum, attr.name.c_str());
				numErrors++;
			}
			if(child[0]->returnType != returnType){
				printf("ERROR(%d): Variable '%s' is of %s but is being initialized with an expression of %s.\n", 
					lineNum, attr.name.c_str(), toString(returnType).c_str(), toString(child[0]->returnType).c_str());
				numErrors++;
			}
		}
		break;
	case PARAMK:
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		}
		break;
	case OPK:
		//case by case---we need a table for this
		//check that left side is valid type
		checkOp();
		
		//set return type
		if(contains(opMap[kind.op][2], TYPEOFLHS)){
			returnType = child[0]->returnType;
			if(contains(opMap[kind.op][2], ARRAYALLOWED) && child[0]->isArray){
				isArray = true;
			}
		}
		else
			returnType = opMap[kind.op][2][0];
		break;
	default:
		cout << "NodeKind undefined in scopeAndTypeSelf";
	}
}

void TreeNode::checkParams(TreeNode* nodePtr, int depth){
	if(nodePtr == NULL){
		if(child.size() > 0 && child[0] != NULL)
			child[0]->scopeAndType(depth);
		
		return;
	}
	
	if(child.size() > 0 && child[0] != NULL){
		if(nodePtr->child.size() == 0 || nodePtr->child[0] == NULL){
			printf("ERROR(%d): Too many parameters passed for function '%s' defined on line %d.\n",
				lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
			
			// Traverse the parameters as normal nodes...
			// CALLK only has one child, so traversal of all children is not necessary
			child[0]->scopeAndType(depth);
		}
		else{			
			//check types
			TreeNode* mySiblingPtr = child[0];
			TreeNode* funSiblingPtr = nodePtr->child[0];
			int siblingNum = 1;
			while(mySiblingPtr != NULL && funSiblingPtr != NULL){
				
				mySiblingPtr->scopeAndTypeSibling(depth);
				
				if(mySiblingPtr->returnType != funSiblingPtr->returnType 
					&& mySiblingPtr->returnType != UNDEFINEDTYPE
					&& funSiblingPtr->returnType != UNDEFINEDTYPE)
				{
					printf("ERROR(%d): Expecting %s in parameter %i of call to '%s' defined on line %d but got %s.\n",
						lineNum, toString(funSiblingPtr->returnType).c_str(), siblingNum, attr.name.c_str(), 
						nodePtr->lineNum, toString(mySiblingPtr->returnType).c_str());
					numErrors++;
				}
				if(mySiblingPtr->isArray != funSiblingPtr->isArray){
					if(mySiblingPtr->isArray){
						printf("ERROR(%d): Not expecting array in parameter %i of call to '%s' defined on line %d.\n",
							lineNum, siblingNum, attr.name.c_str(), nodePtr->lineNum);
						numErrors++;
					}
					else{
						printf("ERROR(%d): Expecting array in parameter %i of call to '%s' defined on line %d.\n",
							lineNum, siblingNum, attr.name.c_str(), nodePtr->lineNum);
						numErrors++;	
					}
				}
				mySiblingPtr = mySiblingPtr->sibling;
				funSiblingPtr = funSiblingPtr->sibling;
				siblingNum++;
			}
			if(mySiblingPtr != NULL){
				printf("ERROR(%d): Too many parameters passed for function '%s' defined on line %d.\n",
					lineNum, attr.name.c_str(), nodePtr->lineNum);
				numErrors++;
				
				mySiblingPtr->scopeAndType(depth);
			}
			else if(funSiblingPtr != NULL){
				printf("ERROR(%d): Too few parameters passed for function '%s' defined on line %d.\n",
					lineNum, attr.name.c_str(), nodePtr->lineNum);
				numErrors++;

			}
		}
	}
	else if(nodePtr->child.size() > 0 && nodePtr->child[0] != NULL){
		//There are no parameters in the call
		
		printf("ERROR(%d): Too few parameters passed for function '%s' defined on line %d.\n",
			lineNum, attr.name.c_str(), nodePtr->lineNum);
		numErrors++;
		
	}
	
}

void TreeNode::checkCall(TreeNode* nodePtr){
	if(child.size() > 0 && child[0] != NULL){
		if(nodePtr->child.size() == 0 || nodePtr->child[0] == NULL){
			printf("ERROR(%d): Too many parameters passed for function '%s' defined on line %d.\n",
				lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
		}
		else{			
			//check types
			TreeNode* mySiblingPtr = child[0];
			TreeNode* funSiblingPtr = nodePtr->child[0];
			int siblingNum = 1;
			while(mySiblingPtr != NULL && funSiblingPtr != NULL){
				if(mySiblingPtr->returnType != funSiblingPtr->returnType 
					&& mySiblingPtr->returnType != UNDEFINEDTYPE
					&& funSiblingPtr->returnType != UNDEFINEDTYPE)
				{
					printf("ERROR(%d): Expecting %s in parameter %i of call to '%s' defined on line %d but got %s.\n",
						lineNum, toString(funSiblingPtr->returnType).c_str(), siblingNum, attr.name.c_str(), 
						nodePtr->lineNum, toString(mySiblingPtr->returnType).c_str());
					numErrors++;
				}
				if(mySiblingPtr->isArray != funSiblingPtr->isArray){
					if(mySiblingPtr->isArray){
						printf("ERROR(%d): Not expecting array in parameter %i of call to '%s' defined on line %d.\n",
							lineNum, siblingNum, attr.name.c_str(), nodePtr->lineNum);
						numErrors++;
					}
					else{
						printf("ERROR(%d): Expecting array in parameter %i of call to '%s' defined on line %d.\n",
							lineNum, siblingNum, attr.name.c_str(), nodePtr->lineNum);
						numErrors++;	
					}
				}
				mySiblingPtr = mySiblingPtr->sibling;
				funSiblingPtr = funSiblingPtr->sibling;
				siblingNum++;
			}
			int myParamCount = child[0]->countSiblings();
			int funParamCount = nodePtr->child[0]->countSiblings();
			if(myParamCount < funParamCount){
				printf("ERROR(%d): Too few parameters passed for function '%s' defined on line %d.\n",
					lineNum, attr.name.c_str(), nodePtr->lineNum);
				numErrors++;
			}
			else if (myParamCount > funParamCount){
				printf("ERROR(%d): Too many parameters passed for function '%s' defined on line %d.\n",
					lineNum, attr.name.c_str(), nodePtr->lineNum);
				numErrors++;
			}
		}
	}
	else if(nodePtr->child.size() > 0 && nodePtr->child[0] != NULL){
		printf("ERROR(%d): Too few parameters passed for function '%s' defined on line %d.\n",
			lineNum, attr.name.c_str(), nodePtr->lineNum);
		numErrors++;
	}
}

bool TreeNode::isAParentOf(TreeNode* p){
	if(p == NULL){
		return false;
	}
	if(p == this){
		return true;
	}
	bool ret = false;
	for(int i = 0; i < child.size() && ret == false; i++){
		if(child[i] == NULL){
			continue;
		}
		ret = ret || child[i]->isAParentOfChild(p);
	}
	return ret;
}


bool TreeNode::isAParentOfChild(TreeNode* p){
	if(p == NULL){
		return false;
	}
	if(p == this){
		return true;
	}
	bool ret = false;
	for(int i = 0; i < child.size() && ret == false; i++){
		if(child[i] == NULL){
			continue;
		}
		ret = ret || child[i]->isAParentOfChild(p);
	}
	TreeNode* siblingPtr = sibling;
	while (siblingPtr != NULL && ret == false){
		ret = ret || siblingPtr->isAParentOf(p);
		siblingPtr = siblingPtr->sibling;
	}
	return ret;
}

void TreeNode::doSymbolTableStuff(){
	TreeNode* nodePtr;
	switch(nodeKind){
	case RECDECLK:
		if(!symTab.insert((char*)attr.name.c_str(), this)){
			nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
			printf("ERROR(%d): Symbol '%s' is already defined at line %d.\n", lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
		}
		returnType = UNDEFINEDTYPE;
		ref = GLOBALREF;
		location = 0;
		if(child[0] != NULL){
			size = child[0]->totalSize();
		}
		else {
			size = 0;
		}
		break;
	case OPK:
	case STMTK:
	case CONSTK:
		ref = VOIDREF;
		break;
	case EXPK: //VARK
		nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
		if (nodePtr == NULL ){
			printf("ERROR(%d): Symbol '%s' is not defined.\n", lineNum, attr.name.c_str());
			numErrors++;
			ref = NOREF;
			size  = 1;
			location = 0;
		}
		else if(nodePtr->nodeKind == FUNDECLK){
			printf("ERROR(%d): Cannot use function '%s' as a variable.\n", lineNum, attr.name.c_str());
			numErrors++;
			returnType = UNDEFINEDTYPE;
			size = nodePtr->size;
			ref = GLOBALREF;
		}
		else if(nodePtr->isAParentOf(this)){
			printf("ERROR(%d): Symbol '%s' is not defined.\n", lineNum, attr.name.c_str());
			numErrors++;
			ref = NOREF;
			size  = 1;
			location = 0;
		}
		else if (nodePtr != NULL){
			size = nodePtr->size;
			location = nodePtr->location;
			ref = nodePtr->ref;
			reference = nodePtr;
		}
		break;
	case CALLK:
		//check that this points to a fundeclk node
		//has type of its return type
		nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
		if (nodePtr == NULL){
			printf("ERROR(%d): Function '%s' is not defined.\n", lineNum, attr.name.c_str());
			numErrors++;
		}
		else if(nodePtr->nodeKind != FUNDECLK){
			printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", lineNum, attr.name.c_str());
			numErrors++;
		}
		else {
			size = nodePtr->size;
			reference = nodePtr;
		}
		ref = NOREF;
		location = 0;
		break;
	case FUNDECLK:
		if(!symTab.insert((char*)attr.name.c_str(), this)){
			nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
			printf("ERROR(%d): Symbol '%s' is already defined at line %d.\n", lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
		}
		ref = GLOBALREF;
		//size is 2 + sizeParams + sizeLocals
		size = 2;
		if(child.size() > 0 && child[0] != NULL){
			size += 1 + child[0]->countSiblings();
		}
		if(child.size() > 1 && child[1] != NULL){
			size += child[1]->totalSize();
		}
		location = 0;
		currLocalLocation = -2;
		//type is kind.decl
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		case VOIDK:
			returnType = VOIDTYPE;
			break;
		}
		break;
	case PARAMK:
		if(!symTab.insert((char*)attr.name.c_str(), this)){
			nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
			printf("ERROR(%d): Symbol '%s' is already defined at line %d.\n", lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
			location = 0;
		}
		else {
			location = currLocalLocation--;
		}
		ref = PARAMREF;
		size = 1; //no matter what type or if it is an array
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		}
		break;
	case DECLK:
		if(!symTab.insert((char*)attr.name.c_str(), this)){
			nodePtr = (TreeNode*)symTab.lookup((char*)attr.name.c_str());
			printf("ERROR(%d): Symbol '%s' is already defined at line %d.\n", lineNum, attr.name.c_str(), nodePtr->lineNum);
			numErrors++;
			if(symTab.depth() == 1){
				ref = GLOBALREF;
			}
			else if(isStatic){
				ref = STATICREF;
			}
			else{
				ref = LOCALREF;
			}
			location = 0;
		} else {				
			//size was set during parsing
			if(symTab.depth() == 1){
				ref = GLOBALREF;
				location = currGlobalLocation;
				currGlobalLocation -= size;
			}
			else if(isStatic){
				ref = STATICREF;
				location = currGlobalLocation;
				currGlobalLocation -= size;
			}
			else{
				ref = LOCALREF;
				location = currLocalLocation;
				currLocalLocation -= size;
			}
			
			if(isArray){
				location--;
			}
		}
		switch(kind.decl){
		case INTK:
			returnType = INTTYPE;
			break;
		case BOOLK:
			returnType = BOOLTYPE;
			break;
		case CHARK:
			returnType = CHARTYPE;
			break;
		case RECORDK:
			returnType = RECORDTYPE;
			break;
		}
		break;
	default:
		cout << "NodeKind undefined in scopeAndTypeSelf";
	}
}

class FunDeclCollection{
public:
		string name;
		DeclKind decl;
		vector<DeclKind> params;
		FunDeclCollection(string n, DeclKind d, vector<DeclKind> p){
			name = n;
			decl = d;
			params = p;
		}
};

TreeNode* prependIOPrototypes(TreeNode* root){
	TreeNode* newRoot = new TreeNode(FUNDECLK, -1, "input", -1, false);
	newRoot->kind.decl = INTK;
	TreeNode* nextChild = NULL;
	
	vector<FunDeclCollection> v = {
		FunDeclCollection{"output", VOIDK, vector<DeclKind>{INTK}},
		FunDeclCollection("inputb", BOOLK, vector<DeclKind>{}),
		FunDeclCollection("outputb", VOIDK, vector<DeclKind>{BOOLK}),
		FunDeclCollection("inputc", CHARK, vector<DeclKind>{}),
		FunDeclCollection("outputc", VOIDK, vector<DeclKind>{CHARK}),
		FunDeclCollection("outnl", VOIDK, vector<DeclKind>{})
	};
	
	for(int i = 0; i < v.size(); i++){
		TreeNode* nextSibling = new TreeNode(FUNDECLK, -1, v[i].name, -1, false);
		nextSibling->kind.decl = v[i].decl;
		for(int j = 0; j < v[i].params.size(); j++){
			nextChild = new TreeNode(PARAMK, -1, "*dummy*", -1, false);
			nextChild->kind.decl = v[i].params[j];
			nextSibling->child.push_back(nextChild);
		}
		newRoot->appendSibling(nextSibling);
	}
	
	newRoot->appendSibling(root);
	return newRoot;
}
#include "treenode.h"
#include "parsercpp.tab.h"
#include <iostream>
#include <sstream>

using namespace std;
extern int currGlobalLocation;

void TreeNode::print(bool printTypes = false){
	print(0, "", printTypes);
	cout << "Offset for end of global space: " << currGlobalLocation << endl;
}

void TreeNode::print(int depth, string prefix, bool printTypes){
	for(int i = 0; i < depth; i++){
		cout << "!   ";
	}
	cout << prefix;
	printSelf(printTypes);
	for(int i = 0; i < child.size(); i++){
		if(child[i] == NULL){
			continue;
		}
		stringstream sstm;
		sstm << "Child: " << i << "  ";
		child[i]->print(depth + 1, sstm.str(), printTypes);
	}
	TreeNode* siblingPtr = sibling;
	int sibNum = 0;
	while (siblingPtr != NULL){
		stringstream sstm;
		sstm << "Sibling: " << sibNum << "  ";
		siblingPtr->printSibling(depth, sstm.str(), printTypes);
		siblingPtr = siblingPtr->sibling;
		sibNum++;
	}
}

void TreeNode::printSibling(int depth, string prefix, bool printTypes){
	for(int i = 0; i < depth; i++){
		cout << "!   ";
	}
	cout << prefix;
	printSelf(printTypes);
	for(int i = 0; i < child.size(); i++){
		if(child[i] == NULL){
			continue;
		}
		stringstream sstm;
		sstm << "Child: " << i << "  ";
		child[i]->print(depth + 1, sstm.str(), printTypes);
	}
}

void TreeNode::printSelf(bool printTypes){
	switch(nodeKind){
	case RECDECLK:
		cout << "Record " << attr.name;
		break;
	case STMTK:
		switch(kind.stmt){
		case COMPOUNDK:
			cout << "Compound";
			break;
		case SELECTIONK:
			cout << "If";
			break;
		case ITERATIONK:
			cout << "While";
			break;
		case RETURNK:
			cout << "Return";
			break;
		case BREAKK:
			cout << "Break";
			break;
		}
		break;
	case EXPK:
		cout << "Id: " << attr.name;
		if(isArray)
			cout << " is array";
		break;
	case CONSTK:
		cout << "Const: ";
		if(kind.constant == NUMC)
			cout << attr.value;
		else if(kind.constant == CHARC){
			char c = attr.value;
			cout << "'" << c << "'";
		}
		else{
			if(attr.value == 0)
				cout << "false";
			else 
				cout << "true";
		}
		break;
	case CALLK:
		cout << "Call: " << attr.name;
		break;
	case FUNDECLK:
		cout << "Func "  << attr.name << " returns type ";
		switch(kind.decl){
		case INTK:
			cout << "int";
			break;
		case BOOLK:
			cout << "bool";
			break;
		case CHARK:
			cout << "char";
			break;
		case RECORDK:
			cout << "record";
			break;
		case VOIDK:
			cout << "void";
			break;
		}
		break;
	case DECLK:
		cout << "Var " << attr.name;
		if(isArray)
			cout << " is array";
		/*
		cout << " of type ";
		switch(kind.decl){
		case INTK:
			cout << "int";
			break;
		case BOOLK:
			cout << "bool";
			break;
		case CHARK:
			cout << "char";
			break;
		case RECORDK:
			cout << "record";
			break;
		}
		*/
		break;
	case PARAMK:
		cout << "Param " << attr.name;
		if(isArray)
			cout << " is array";
		/*
		cout << " of type ";
		switch(kind.decl){
		case INTK:
			cout << "int";
			break;
		case BOOLK:
			cout << "bool";
			break;
		case CHARK:
			cout << "char";
			break;
		case RECORDK:
			cout << "record";
		}
		*/
		break;
	case OPK:
		switch(kind.op){
		case ASSK:
		case ADDASSK:
		case SUBASSK:
		case MULASSK:
		case DIVASSK:
		case INCK:
		case DECK:
			cout << "Assign: " << attr.name;
			break;
		default:
			cout << "Op: " << attr.name;
		}
		break;
	default:
		cout << "NodeKind undefined in printSelf";
	}
	
	if(printTypes){
		int displaySize = size;
		if(nodeKind == FUNDECLK || nodeKind == CALLK){
			displaySize = -size;
		}
		switch(ref){
		case GLOBALREF:
			cout << " [ref: Global, size: " << displaySize << ", loc: " << location << "]";
			break;
		case LOCALREF:
			cout << " [ref: Local, size: " << displaySize << ", loc: " << location << "]";
			break;
		case PARAMREF:
			cout << " [ref: Param, size: " << displaySize << ", loc: " << location << "]";
			break;
		case STATICREF:
			cout << " [ref: Static, size: " << displaySize << ", loc: " << location << "]";
			break;
		case NOREF:
			cout << " [ref: None, size: " << displaySize << ", loc: " << location << "]";
			break;
		case VOIDREF:
			break;
		}
	}
	
	if(printTypes){
		switch(returnType){
		case BOOLTYPE:
			cout << " [type bool]";
			break;
		case INTTYPE:
			cout << " [type int]";
			break;
		case CHARTYPE:
			cout << " [type char]";
			break;
		case RECORDTYPE:
			cout << " [type recordtype]";
			break;
		case VOIDTYPE:
			cout << " [type void]";
			break;
		case UNDEFINEDTYPE:
			cout << " [undefined type]";
			break;
		case ARRAYALLOWED:
		case ARRAYREQUIRED:
		case TYPEOFLHS:
			break;
		}
	}
	
	cout << " [line: " << lineNum << "]" << endl;
}
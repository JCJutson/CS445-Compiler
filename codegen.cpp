#include"codegen.h"
#include"code.h"
#include<iostream>
#include<string>
#include<stack>

void genDecl( TreeNode * tree);
void genConst( TreeNode * tree);
void genCall( TreeNode * tree);
void genOp(TreeNode*);
void genStmt(TreeNode*);
void genExp(TreeNode*);
void genFunDecl(TreeNode* tree);
void recursiveCodeGen(TreeNode*);
void nonrecursiveCodeGen(TreeNode* tree);
void genInit(TreeNode* tree);
void recursiveInit(TreeNode* tree);

int tOffset = 0;
extern int currGlobalLocation;

char* cstring(string s){
	if(s.empty()){
		return "";
	}
	else{
		return (char*)s.c_str();
	}
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the codeFile file
 */
void codeGen(TreeNode* tree, string fileName){
	codeFile = fopen(cstring(fileName), "w");
	
	string s = "File: " + fileName;
	
	emitComment("TINY Compilation to TM Code");
	emitComment(cstring(s));
	
	/* generate standard prelude */
	/*emitComment("Standard prelude:");
	emitRM("LD",FP,0,AC,"load maxaddress from location 0");
	emitRM("ST",AC,0,AC,"clear location 0");
	emitComment("End of standard prelude.");
	*/
	/* generate code for TINY program */
	emitSkip(1);
	recursiveCodeGen(tree);
	
	backPatchAJumpToHere("LDA", PC, 0, "Jump to init [backpatch]");
	genInit(tree);
}

void genInit(TreeNode* tree){
	emitComment("INIT");
	emitRM("LD", 0, 0, 0, "Set the global pointer");
	
	emitRM("LDA", FP, currGlobalLocation, GP, "set first frame at end of globals");
	
	emitRM("ST", 1, 0, 1, "store old fp (point to self)");
	emitComment("INIT GLOBALS AND STATICS");
	recursiveInit(tree);
	emitComment("END INIT GLOBALS AND STATICS");
	emitRM("LDA", AC, 1, 7, "Return address in ac");
	emitGotoAbs(((TreeNode*) tree->symTab.lookup("main"))->codeLineNum, "Jump to main");
	emitRO("HALT", 0, 0, 0, "DONE!");
	emitComment("END INIT");
}

void recursiveInit(TreeNode* tree){
	
	if(tree->nodeKind == DECLK){
		if(tree->isArray){
			emitRM("LDC", AC, tree->size - 1, AC3, "load size of array", cstring(tree->attr.name));
			emitRM("ST", AC, tree->location + 1, GP, "save size of array", cstring(tree->attr.name));
		}
		else if(tree->child.size() > 0 && tree->child[0] != NULL){
			recursiveCodeGen(tree->child[0]);
			emitRM("ST", AC, tree->location,
				(tree->ref == GLOBALREF) ? GP : FP, 				
				"Store variable", cstring(tree->attr.name));
		}
	}
	
	if(tree->sibling != NULL){
		recursiveInit(tree->sibling);
	}
}

/* Procedure recursiveCodeGen recursively generates code by
 * tree traversal
 */
void recursiveCodeGen(TreeNode* tree)
{ 
	if (tree != NULL)
	{ 
		nonrecursiveCodeGen(tree);
		recursiveCodeGen(tree->sibling);
	}
}

void nonrecursiveCodeGen(TreeNode* tree){
	if (tree != NULL)
	{ 
		switch (tree->nodeKind) {
		case STMTK:
			genStmt(tree);
			break;
		case OPK:
			genOp(tree);
			break;
		case CONSTK:
			genConst(tree);
			break;
		case CALLK:
			genCall(tree);
			break;
		case EXPK:
			genExp(tree);
			break;
		case DECLK:
			genDecl(tree);
			break;
		case FUNDECLK:
			genFunDecl(tree);
			break;
		case PARAMK:
		case RECDECLK:
		default:
			break;
		}
	}
}

void genFunDecl(TreeNode* tree){
	tree->codeLineNum = emitSkip(0);
	tOffset = -tree->size;
	emitComment("** ** ** ** ** ** ** ** ** ** ** **");
	emitComment("FUNCTION", cstring(tree->attr.name));
	emitRM("ST", 3, -1, 1, "Store return address");
	if(tree->attr.name == "input"){
		emitRO("IN", 2, 2, 2, "Grab int input");
	}
	else if(tree->attr.name == "output"){
		emitRM("LD", AC, -2, FP, "Load parameter");
		emitRO("OUT", 3, 3, 3, "Output integer");
		emitRM("LDC", RT, 0, 6, "Set return to 0");
	}
	else if(tree->attr.name == "inputb"){
		emitRO("INB", 2, 2, 2, "Grab bool input");
	}
	else if(tree->attr.name == "outputb"){
		emitRM("LD", AC, -2, FP, "Load parameter");
		emitRO("OUTB", 3, 3, 3, "Output bool");
		emitRM("LDC", RT, 0, 6, "Set return to 0");
	}
	else if(tree->attr.name == "inputc"){
		emitRO("INC", 2, 2, 2, "Grab char input");
	}
	else if(tree->attr.name == "outputc"){
		emitRM("LD", AC, -2, FP, "Load parameter");
		emitRO("OUTC", 3, 3, 3, "Output char");
		emitRM("LDC", RT, 0, 6, "Set return to 0");
	}
	else if(tree->attr.name == "outnl"){
		emitRO("OUTNL", 3, 3, 3, "Output a newline");
	}
	else{
		recursiveCodeGen(tree->child[1]);
		emitComment("Add standard closing in case there is no return statement");
		emitRM("LDC", RT, 0, AC3, "Set return value to 0");
	}
	
	emitRM("LD", AC, -1, FP, "Load return address");
	emitRM("LD", FP, 0, FP, "Adjust fp");
	emitRM("LDA", PC, 0, AC, "Return");		
	emitComment("END FUNCTION", cstring(tree->attr.name));
	emitComment("");
	tOffset = 0;
}

stack<int> breakLocations;

/* Procedure genStmt generates code at a statement node */
void genStmt( TreeNode * tree)
{ 
	TreeNode * p1, * p2, * p3;
	int loc1, loc2, loc3, loc4;
	static int locForBreaks;

	switch (tree->kind.stmt) {
	case COMPOUNDK:
		emitComment("COMPOUND");
		recursiveCodeGen(tree->child[0]);
		emitComment("Compound Body");
		recursiveCodeGen(tree->child[1]);
		emitComment("END COMPOUND");
		break;
	case RETURNK:
		emitComment("RETURN");
		if(tree->child.size() > 0 && tree->child[0] != NULL){
			recursiveCodeGen(tree->child[0]);
			emitRM("LDA", RT, 0, AC, "Copy result to rt register");
		}
		emitRM("LD", AC, -1, FP, "Load return address");
		emitRM("LD", FP, 0, FP, "Adjust fp");
		emitRM("LDA", PC, 0, AC, "Return");
		break;
	case BREAKK:
		emitComment("BREAK");
		emitGotoAbs(breakLocations.top(), "break");
		break;
	case SELECTIONK :
		emitComment("IF");
		
		//test
		recursiveCodeGen(tree->child[0]);
		// if false go to loc1
		loc4 = emitSkip(1);
		
		emitComment("THEN");
		recursiveCodeGen(tree->child[1]);
		if(tree->child[2] != NULL){
			loc3 = emitSkip(1);
		}
		//go to loc2 (after the else)
		
		//this is loc1:
		backPatchAJumpToHere("JZR", AC, loc4, "Jump around the THEN if false [backpatch]");
		
		if(tree->child[2] != NULL){
			emitComment("ELSE");
			recursiveCodeGen(tree->child[2]);
			
			//this is loc2
			backPatchAJumpToHere(loc3, "Jump around the ELSE [packpatch]");
		}
	/*
		p1 = tree->child[0] ;
		p2 = tree->child[1] ;
		p3 = tree->child[2] ;

		recursiveCodeGen(p1);
		savedLoc1 = emitSkip(1) ;
		emitComment("if: jump to else belongs here");

		recursiveCodeGen(p2);
		savedLoc2 = emitSkip(1) ;
		emitComment("if: jump to end belongs here");
		currentLoc = emitSkip(0) ;
		emitBackup(savedLoc1) ;
		emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
		emitRestore() ;

		recursiveCodeGen(p3);
		currentLoc = emitSkip(0) ;
		emitBackup(savedLoc2) ;
		emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
		emitRestore() ;
*/	
		emitComment("ENDIF");
	break; /* if_k */

	case ITERATIONK:
		emitComment("WHILE");
		loc1 = emitSkip(0);
		recursiveCodeGen(tree->child[0]);
		//if true, go to loc3
		emitRM("JNZ", AC, 1, PC, "Jump to while part"); //the code body
		//loc4: go to loc2
		loc4 = emitSkip(1);
		breakLocations.push(loc4);
		
		//code body
		recursiveCodeGen(tree->child[1]);
		emitGotoAbs(loc1, "go to beginning of loop");
		//go to loc1
		backPatchAJumpToHere(loc4, "Jump past loop [backpatch]");
		breakLocations.pop();
		emitComment("ENDWHILE");
		break; 
	default:
		break;
    }
} /* genStmt */

void emitLoadAddressBaseArray(int reg, TreeNode* tree){
	char* instruction = (tree->ref == PARAMREF) ? (char*)"LD" : (char*)"LDA";
	emitRM(instruction, reg, tree->reference->location, 
			(tree->ref == GLOBALREF) ? GP : FP, 
			"Load address of base of array",
			cstring(tree->attr.name));
}

void genOp(TreeNode* tree){
	int loc;
	TreeNode* nodePtr;
	string comment;
	
	
	switch(tree->kind.op){
    case ASSK:
        emitComment("EXPRESSION");
		if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitRM("ST", AC, tOffset--, FP, "Save index");
			
			recursiveCodeGen(tree->child[1]);
			
			emitRM("LD", AC1, ++tOffset, FP, "Restore index");
			
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC1, "Compute offset of value");
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
			//66:     ST  3,0(5)	Store variable a
		}
		else{
			recursiveCodeGen(tree->child[1]);
			if(tree->child[0]->reference != NULL){
				emitRM("ST", AC, tree->child[0]->reference->location,
					(tree->child[0]->ref == GLOBALREF) ? GP : FP, 				
					"Store variable", cstring(tree->child[0]->attr.name));
			}	
			else{
				cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
			}
		}
		break;	
	case ADDASSK:
		if(tree->child[0]->reference != NULL){
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("ADD", AC, AC1, AC, "Op +=");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}	
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitRM("ST", AC, tOffset--, FP, "Save index");
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, ++tOffset, FP, "Restore index");
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC1, "Compute offset of value");
			
			emitRM("LD", AC1, 0, AC2,
					"Load array value from", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("ADD", AC, AC1, AC, "Op +=");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}
		else {
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;	
	case SUBASSK:
		if(tree->child[0]->reference != NULL){
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("SUB", AC, AC1, AC, "Op -=");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitRM("ST", AC, tOffset--, FP, "Save index");
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, ++tOffset, FP, "Restore index");
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC1, "Compute offset of value");
			
			emitRM("LD", AC1, 0, AC2,
					"Load array value from", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("SUB", AC, AC1, AC, "Op -=");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}		
		else{
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;	
	case MULASSK:
		if(tree->child[0]->reference != NULL){
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("MUL", AC, AC1, AC, "Op *=");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitRM("ST", AC, tOffset--, FP, "Save index");
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, ++tOffset, FP, "Restore index");
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC1, "Compute offset of value");
			
			emitRM("LD", AC1, 0, AC2,
					"Load array value from", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("MUL", AC, AC1, AC, "Op *=");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}
		else{
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;	
	case DIVASSK:
		if(tree->child[0]->reference != NULL){
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("DIV", AC, AC1, AC, "Op /=");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitRM("ST", AC, tOffset--, FP, "Save index");
			recursiveCodeGen(tree->child[1]);
			emitRM("LD", AC1, ++tOffset, FP, "Restore index");
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC1, "Compute offset of value");
			
			emitRM("LD", AC1, 0, AC2,
					"Load array value from", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("DIV", AC, AC1, AC, "Op /=");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}
		else{
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;	
	case ADDK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("ADD", AC, AC1, AC, "Op +");
		break;
	case SUBK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("SUB", AC, AC1, AC, "Op -");
		break;
	case INCK:
		if(tree->child[0]->reference != NULL){
			emitRM("LD", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("LDA", AC, 1, AC, "Op ++");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}	
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			
			emitRO("SUB", AC2, AC2, AC, "Compute offset of value");
			
			emitRM("LD", AC, 0, AC2,
					"Load lhs variable", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("LDA", AC, 1, AC, "Op ++");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}
		else {
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;	
	case DECK:
		if(tree->child[0]->reference != NULL){
			emitRM("LD", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP,				
				"load lhs variable", cstring(tree->child[0]->attr.name));
			emitRO("LDA", AC, -1, AC, "Op --");
			emitRM("ST", AC, tree->child[0]->reference->location, 
				(tree->child[0]->ref == GLOBALREF) ? GP : FP, 
				"Store variable", cstring(tree->child[0]->attr.name));
		}	
		else if(tree->child[0]->nodeKind == OPK && tree->child[0]->kind.op == ARRACCK){
			//This should happen if the left side is '['
			recursiveCodeGen(tree->child[0]->child[1]);
			emitLoadAddressBaseArray(AC2, tree->child[0]->child[0]);
			emitRO("SUB", AC2, AC2, AC, "Compute offset of value");
			
			emitRM("LD", AC, 0, AC2,
					"Load array value from", cstring(tree->child[0]->child[0]->attr.name));
			
			emitRO("LDA", AC, -1, AC, "Op --");
			
			emitRM("ST", AC, 0, AC2, 
				"Store variable", cstring(tree->child[0]->child[0]->attr.name));
		}
		else {
			cout << "Reference for " << tree->child[0]->attr.name << " is NULL." << endl;
		}
		break;
	case MULK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("MUL", AC, AC1, AC, "Op *");
		break;	
	case DIVK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("DIV", AC, AC1, AC, "Op /");
		break;	
	case MODK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("DIV", AC2, AC1, AC, "Op %");
		emitRO("MUL", AC2, AC2, AC, "Op %");
		emitRO("SUB", AC, AC1, AC2, "Op %");
		break;	
	case NEGK:
		recursiveCodeGen(tree->child[0]);
		emitRM("LDC", AC1, -1, AC3, "Load -1 for unary -");
		emitRO("MUL", AC, AC1, AC, "Op unary -");
		break;
	case ORK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("OR", AC, AC1, AC, "Op or");
		break;
	case ANDK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("AND", AC, AC1, AC, "Op and");
		break;	
	case NOTK:
		recursiveCodeGen(tree->child[0]);
		emitRM("LDC", AC1, 0, 0, "Load 0");
		emitRO("TEQ", AC, AC, AC1, "Op !");
		break;	
	case LESSEQK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TLE", AC, AC1, AC, "Op <=");
		break;	
	case LESSK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TLT", AC, AC1, AC, "Op <");
		break;	
	case EQK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TEQ", AC, AC1, AC, "Op ==");
		break;	
	case GRTK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TGT", AC, AC1, AC, "Op >");
		break;	
	case GRTEQK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TGE", AC, AC1, AC, "Op >=");
		break;	
	case NOTEQK:
		recursiveCodeGen(tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("TNE", AC, AC1, AC, "Op !=");
		break;
	case ARRSIZEK:
		emitLoadAddressBaseArray(AC, tree->child[0]);
		emitRM("LD", AC, 1, AC,				
			"Load size of array", cstring(tree->child[0]->attr.name));
		break;	
	case RANDK:
		recursiveCodeGen(tree->child[0]);
		emitRO("RND", AC, AC, AC, "Op ?");
		break;	
	case ARRACCK:
		emitLoadAddressBaseArray(AC, tree->child[0]);
		emitRM("ST", AC, tOffset--, FP, "Save left side");
		recursiveCodeGen(tree->child[1]);
		emitRM("LD", AC1, ++tOffset, FP, "Load left into ac1");
		emitRO("SUB", AC, AC1, AC, "compute location from index");
		emitRM("LD", AC, 0, AC, "Load array element");
		break;	
	case DOTK:
		break;
	}
	
}

void genConst( TreeNode * tree){
	switch(tree->kind.constant){
	case NUMC:
		emitRM("LDC", AC, tree->attr.value, AC3, "Load integer constant");
		break;
	case BOOLC:
		emitRM("LDC", AC, tree->attr.value, AC3, "Load boolean constant");
		break;
	case CHARC:
		emitRM("LDC", AC, tree->attr.value, AC3, "Load character constant");
		break;
	}
}

void genCall( TreeNode * tree){
	emitComment("EXPRESSION");
	emitComment("                      Begin call to ", cstring(tree->attr.name));
	int ghostFP = tOffset;
	emitRM("ST", FP, ghostFP, FP, "Store old fp in ghost frame");
	tOffset -= 3;
	//traverse params
	int paramNum = 1;
	TreeNode* nodePtr = tree->child[0];
	while(nodePtr != NULL){
		emitComment("                      Load param", cstring(to_string((long long)paramNum)));
		nonrecursiveCodeGen(nodePtr);
		emitRM("ST", AC, ghostFP - paramNum - 1, FP, "Store parameter");
		nodePtr = nodePtr->sibling;
		paramNum++;
	}
	emitComment("                      Jump to", cstring(tree->attr.name));
	emitRM("LDA", FP, ghostFP, FP, "Load address of new frame");
	emitRM("LDA", AC, 1, PC, "Return address in ac");
	emitRMAbs("LDA", PC, tree->reference->codeLineNum, "CALL", cstring(tree->attr.name));
	emitRM("LDA", AC, 0, RT, "Save the result in ac");
	emitComment("                      End call to", cstring(tree->attr.name));
	tOffset += 3;
}

/* Procedure genExp generates code at an expression node */
void genExp( TreeNode * tree)
{
	TreeNode * p1, * p2;
	switch (tree->kind.exp) {
	case VARK:
		if(tree->reference == NULL){
			cout << "Reference to " << tree->attr.name << "is NULL in genExp" << endl;
		}
		else{
			if(tree->isArray){
				emitLoadAddressBaseArray(AC, tree);
			}
			else{
				emitRM("LD", AC, tree->reference->location, 
				(tree->ref == GLOBALREF) ? GP : FP,				
				"Load variable", cstring(tree->attr.name));
			}
		}
	}
} /* genExp */

void genDecl( TreeNode * tree){
	if(tree->ref != GLOBALREF){
		if(tree->isArray){
			emitRM("LDC", AC, tree->size - 1, AC3, "load size of array", cstring(tree->attr.name));
			emitRM("ST", AC, tree->location + 1, FP, "save size of array", cstring(tree->attr.name));
		}
		else if(tree->child.size() > 0 && tree->child[0] != NULL){
			recursiveCodeGen(tree->child[0]);
			emitRM("ST", AC, tree->location,
				(tree->ref == GLOBALREF) ? GP : FP, 				
				"Store variable", cstring(tree->attr.name));
		}
	}
}

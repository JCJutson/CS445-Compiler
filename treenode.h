#ifndef TREENODE_H
#define TREENODE_H
#include<vector>
#include<string>
#include<map>
#include"symtab.h"
#include"scanType.h"

const int MAXCHILDREN = 3;

enum NodeKind {
	OPK,
	RECDECLK,
	DECLK,
	STMTK,
	EXPK,
	CONSTK,
	CALLK,
	FUNDECLK,
	PARAMK
};

enum DeclKind {
	INTK,
	BOOLK,
	CHARK,
	RECORDK,
	VOIDK
};

enum StmtKind {
	COMPOUNDK,
	SELECTIONK,
	ITERATIONK,
	RETURNK,
	BREAKK
};

enum ExpKind {
	VARK
};

enum OpKind {
	ORK,
	ANDK,
	NOTK,
	LESSEQK,
	LESSK,
	EQK,
	GRTK,
	GRTEQK,
	NOTEQK,
	ADDK,
	SUBK,
	MULK,
	DIVK,
	MODK,
	NEGK,
	ARRSIZEK,
	RANDK,
	ARRACCK,
	DOTK,
	ASSK,
	ADDASSK,
	SUBASSK,
	MULASSK,
	DIVASSK,
	INCK,
	DECK
};

enum ConstantKind {
	BOOLC,
	NUMC,
	CHARC
};

enum ReturnType {
	BOOLTYPE,
	INTTYPE,
	CHARTYPE,
	RECORDTYPE,
	VOIDTYPE,
	UNDEFINEDTYPE,
	ARRAYALLOWED,
	ARRAYREQUIRED,
	TYPEOFLHS
};

enum RefScope {
	LOCALREF,
	GLOBALREF,
	PARAMREF,
	STATICREF,
	NOREF,
	VOIDREF
};

typedef struct SecondaryKind           // subtype of type
{
    DeclKind decl;                     // used when DeclK
    StmtKind stmt;                     // used when StmtK
    ExpKind exp;                       // used when ExpK
    OpKind op;
    ConstantKind constant;
} SecondaryKind;

// extra properties about the node depending on type of the node
typedef struct Attributes                                 // relevant data to type -> attr
{
	OpKind op;                         // type of token (same as in bison)
	int value;                         // used when an integer constant or boolean
	unsigned char cvalue;               // used when a character
	std::string name;                        // used when IdK
} Attributes;   

class TreeNode
{
public:
  // connectivity in the tree
  std::vector<TreeNode*> child;   // children of the node
  TreeNode* sibling;              // siblings for the node
  TreeNode* reference;			  // reference for ids and calls
  int codeLineNum;

  // what kind of node
  int lineNum;                            // linenum relevant to this node
  NodeKind nodeKind;                     // type of node
                                    // subtype of type
  SecondaryKind kind;
  Attributes attr;                              
  ReturnType returnType;
  int size;
  int location;
  RefScope ref;
  bool isBool;
  bool isArray;                          // is this an array
  bool isRecord;                         // is this a record
  bool isStatic;                         // is staticly allocated?
  bool isConstant;
  bool isStatementUnderFunction; 		// if set for a compound statement, we should not enter a new scope for this.

	void printSelf(bool printTypes);
  void print(bool printTypes);
  void print(int depth, std::string prefix, bool printTypes);
  void printSibling(int depth, std::string prefix, bool printTypes);
  void scopeAndType();
  void scopeAndType(int depth);
  void scopeAndTypeSibling(int depth);
  bool isAParentOf(TreeNode*);
  bool isAParentOfChild(TreeNode*);
  
  TreeNode();
  TreeNode(NodeKind nk, TokenData* tokenData);
  TreeNode(NodeKind nk, int lineNum, std::string name, int value, bool isaBool);
  void appendSibling(TreeNode*);
  int countSiblings();
  int totalSize();
  int totalSizeSibling();
  
  void checkParams(TreeNode* nodePtr, int depth);
  void checkCall(TreeNode*);

	static SymTab symTab;
  
  // even more semantic stuff will go here in later assignments.
private:

	static std::map<OpKind, std::vector< std::vector<ReturnType> > > opMap;
	void typeSelf();
	void checkOp();
	void doSymbolTableStuff();
	void enterIfNecessary();
	void leaveIfNecessary();
	void initializeMap();
};

TreeNode* prependIOPrototypes(TreeNode* root);


#endif

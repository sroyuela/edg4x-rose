// This is part of a sub-project within ROSE to define a higher level interface 
// for the construction of the AST.



/* *********** Notes on the construction of IR nodes in ROSE from scratch **********

   Building IR nodes in ROSE is a matter of following a few rules, most IR nodes
are easy to build, but a few are more complex.  This High Level Intefce defines simple
functions to call to simplify how an AST can be built up from scratch.
General rules are:
   1) Use the new IR nodes constructors that do NOT take a Sg_File_Info pointer.
      E.g. SgForStatement (SgStatement *test, SgExpression *increment, SgBasicBlock *loop_body)

   2) Always set the scope if the scope is explicitly defined in that IR node 
      (call SgStatement::hasExplicitScope() to test for this).

   3) The parent of the IR nodes should be set if it is a SgLocatedNode or a for a few case of a SgSupport IR node:
      (SgInitializedName, SgFile, SgDirectory, Sg_File_Info, SgModifier, SgSymbolTable, SgTemplateArgument, SgTemplateArgumentList,
      SgFunctionParameterTypeList, SgBaseClass, SgTemplateParameter, SgTemplateParameterList, SgTypedefSeq, (and maybe a few others)).

   4) Set the source code poisition, call SgLocatedNode::set_startOfConstruct() with a valid
      Sg_File_Info object. Also, call SgLocatedNode::set_endOfConstruct() with a valid 
      Sg_File_Info object.  Note that all Sg_File_Info object must be unique (can not be shared).
      
   5) For SgExpression objects, if the expression is an operator, then the position location of
      the operator need be set.  The operator position is optional, so need not be set for 
      transformations (I think).

   6) Declarations which generate symbols need to be added to symbol tables (use member function 
      hasAssociatedSymbol() to check if a symbol is defined for a declaration).

For source location position information, if an AST is being constructed from scratch, then it
does not correspond to a position in the source code and the Sg_File_Info object should be
generated by a static member function Sg_File_Info::generateDefaultFileInfoForTransformationNode().

 */





namespace HighLevelInterface {

SgScopeStatementPtrList currentScope;



void buildInitializedName();
void buildVariableDeclaration();
void buildValueExpression();
void buildAddOperator();

// Functions to set details required to pass later consistancy testing...
void setStatementDetails   ( SgStatement*   stmt,        SgNode* parent, SgScopeStatement* scope );
void setExpressionDetails  ( SgExpression*  expr,        SgNode* parent );
void setLocatedNodeDetails ( SgLocatedNode* locatedNode, SgNode* parent );
  
 
}

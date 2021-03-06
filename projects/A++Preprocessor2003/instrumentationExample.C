// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "Cxx_GrammarTreeTraversalClass.h"

// this is required as a way to hand the SgProject to the prependSourceCode, 
// appendSourceCode, replaceSourceCode, and insertSourceCode functions.
SgProject* globalProject = NULL;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
SimpleInstrumentation::visit ( SgNode* astNode )
   {
  // Demonstrate and test append mechanism for statements

  // printf ("In assemblyFunction(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

     if (isSgFunctionDeclaration(astNode) != NULL)
        {
       // printf ("Found a function declaration \n");
          SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration(astNode);
          SgName sageName = functionDeclarationStatement->get_name();
          string functionNameString = sageName.str();

       // Make sure this is the "main" function before we insert new code
          if (functionNameString == "main")
             {
               string globalDeclarations = "int k;";
               string functionSource = "";
	       string localDeclarations = "\
void myTimerFunctionStart(void) \n\
   { \n\
     int xyzVariable; \n\
   } \n\n\
void myTimerFunctionEnd(void) \n\
   { \n\
     int xyzVariable; \n\
   } \n\
";
               localDeclarations    = "int VARIABLE_DECLARATIONS_MARKER_START; \n" + 
                                      localDeclarations +
                                     "\nint VARIABLE_DECLARATIONS_MARKER_END;\n";

               bool isADeclaration = TRUE;
            // bool isADeclaration = FALSE;

            // Place this code before the first function definition
            // astNode->prependSourceCode(*globalProject,functionSource,globalDeclarations,isADeclaration);
            // astNode->appendSourceCode (*globalProject,functionSource,globalDeclarations,isADeclaration);
//             astNode->prependSourceCode(*globalProject,functionSource,localDeclarations,globalDeclarations,isADeclaration);

            // printf ("Processed \"main\" function \n");
            // ROSE_ABORT();
	     }

       // printf ("Processed function in example5.C \n");
       // ROSE_ABORT();
        }

     if (astNode->variant() == BASIC_BLOCK_STMT)
        {
       // printf ("Found a SgBasicBlock \n");
          const int SIZE_OF_BLOCK = 1;
          if (isSgBasicBlock(astNode)->get_statements().size() > SIZE_OF_BLOCK)
             {
               string globalDeclarations = "void myTimerFunctionStart(); void myTimerFunctionEnd();";
	       string localDeclarations   = "int i;";
               string codeAtTopOfBlock    = "myTimerFunctionStart();";
               string codeAtBottomOfBlock = "myTimerFunctionEnd();";

               codeAtTopOfBlock    = "int VARIABLE_DECLARATIONS_MARKER_START; \n" + 
                                      codeAtTopOfBlock +
                                     "\nint VARIABLE_DECLARATIONS_MARKER_END;\n";

               codeAtBottomOfBlock = "int VARIABLE_DECLARATIONS_MARKER_START; \n" + 
                                      codeAtBottomOfBlock +
                                     "\nint VARIABLE_DECLARATIONS_MARKER_END;\n";

            // Build a new AST fragment and insert it into the AST
               bool locateNewCodeAtTop = TRUE;
            // bool isADeclaration = FALSE;
               bool isADeclaration = TRUE;
               astNode->insertSourceCode(*globalProject,codeAtTopOfBlock,localDeclarations,globalDeclarations,locateNewCodeAtTop,isADeclaration);
               locateNewCodeAtTop = FALSE;
               astNode->insertSourceCode(*globalProject,codeAtBottomOfBlock,localDeclarations,globalDeclarations,locateNewCodeAtTop,isADeclaration);
             }
        }
   }

int
main ( int argc, char * argv[] )
   {
  // Build the project object which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     int EDG_FrontEndErrorCode = 0;
     SgProject sageProject (argc,argv,EDG_FrontEndErrorCode);

  // Warnings from EDG processing are OK but not errors
     ROSE_ASSERT (EDG_FrontEndErrorCode <= 3);

  // Traverse the AST an apply the function
  // TraverseAST::traverse ( sageProject, transformFunction );

  // This is the way we pass in the project that our query requires (temporary code)
  // accumulatorValue.project = &sageProject;
     globalProject = &sageProject;

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( &sageProject, postorder );

  // Generate the final C++ source code from the potentially modified SAGE AST
     sageProject.unparse();

  // What remains is to run the specified compiler (typically the C++ compiler) using 
  // the generated output file (unparsed and transformed application code) to generate
  // an object file.
     int finalCombinedExitStatus = sageProject.compileOutput();

  // return any errors generated by the backend compilation of the modified source code
     return finalCombinedExitStatus;
   }







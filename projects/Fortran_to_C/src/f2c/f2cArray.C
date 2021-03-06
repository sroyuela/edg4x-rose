#include "f2c.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

/******************************************************************************************************************************/
/* 
  Replace all array subscripts into single dimension
  This section is for the declaration.  Multi-dimensional arrays are converted to single-dimensional array.
*/
/******************************************************************************************************************************/
void Fortran_to_C::linearizeArrayDeclaration(SgArrayType* originalArrayType)
{
  // Get dim_info
  SgExprListExp* dimInfo = originalArrayType->get_dim_info();
  // Get dim list
  SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();

  SgExpression* newDimExpr; 
  Rose_STL_Container<SgExpression*>::iterator j =  dimExpressionPtrList.begin();
  while(j != dimExpressionPtrList.end())
  {
    SgExpression* indexExpression;
    SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(*j);
    /*
      This is for the 1st type of array declaration: a(10,15,20)
    */
    if(subscriptExpression == NULL)
    {
      indexExpression = deepCopy(*j);
    }
    else
    /*
      This is for the 2nd type of array declaration: a(1:10,5:15,10:20)
      Actual dimension size = upperBound - lowerBound + 1
    */
    {
      indexExpression = buildSubtractOp(deepCopy(subscriptExpression->get_upperBound()),
                                        deepCopy(subscriptExpression->get_lowerBound()));
      indexExpression = buildAddOp(indexExpression,buildIntVal(1));
    } 

    /*
      Total array size is equal to the multiplication of all individual dimension size.
    */
    if(j != dimExpressionPtrList.begin()){
        newDimExpr = buildMultiplyOp(newDimExpr, indexExpression);
    }
    else
    /*
      If it's first dimension, array size is just its first dimension size.
    */
    {
      newDimExpr = indexExpression;
    }
    ++j;
  }
  // calling set_index won't replace the default index expression.  I have to delete the default manually.
  deepDelete(originalArrayType->get_index());
  originalArrayType->set_index(newDimExpr);
  newDimExpr->set_parent(originalArrayType);
  originalArrayType->set_rank(1); 
}


/******************************************************************************************************************************/
/* 
  Replace all array subscripts into single dimension
  This section is for the array references.  
  Subscripts for multi-dimensional arrays are transformed into single-dimensional array subscript.
*/
/******************************************************************************************************************************/
void Fortran_to_C::linearizeArraySubscript(SgPntrArrRefExp* pntrArrRefExp)
{
  // get lhs operand
  SgVarRefExp*  arrayName = isSgVarRefExp(pntrArrRefExp->get_lhs_operand());
  // get array symbol
  SgVariableSymbol* arraySymbol = arrayName->get_symbol();
  // get array type and dim_info
  SgArrayType* arrayType = isSgArrayType(arraySymbol->get_type());
  ROSE_ASSERT(arrayType);
  SgExprListExp* dimInfo = arrayType->get_dim_info();

  // get rhs operand
  SgExprListExp*  arraySubscript = isSgExprListExp(pntrArrRefExp->get_rhs_operand());
  /*
    No matter it is single or multi dimensional array,  pntrArrRefExp always has a
    child, SgExprListExp, to store the subscript information.
  */
  if(arraySubscript != NULL)
  {
    // get the list of subscript
    SgExpressionPtrList subscriptExprList = arraySubscript->get_expressions();
    // get the list of dimension inforamtion from array definition.
    SgExpressionPtrList dimExpressionPtrList = dimInfo->get_expressions();

    // Create new SgExpressionPtrList for the linearalized array subscript. 
    SgExpressionPtrList newSubscriptExprList;

    // rank info has to match between subscripts and dim_info
    ROSE_ASSERT(arraySubscript->get_expressions().size() == dimInfo->get_expressions().size());
   
    /*
      The subscript conversion is following this example:
      case 1:
      dimension a(d1,d2,d3,d4)    ====>   dimension a(d1*d2*d3*d4)
      a(s1,s2,s3,s4)              ====>   a(s1-1 + d1*(s2-1 + d2*( s3-1 + d3*(s4-1))))

      case 2:
      dimension a(d1L:d1H,d2L:d2H)    ====>   dimension a((d1H-d1L+1)*(d2H-d2L+1))
      a(s1,s2)              ====>   a(s1-d1L + (d1H-d1L+1)*(s2-d2L)) 
    */ 
    Rose_STL_Container<SgExpression*>::reverse_iterator j1 =  subscriptExprList.rbegin();
    Rose_STL_Container<SgExpression*>::reverse_iterator j2 =  dimExpressionPtrList.rbegin();
    // Need to know current size of both current and previous dimension
    SgExpression* newSubscript;
    while((j1 != subscriptExprList.rend()) && (j2 != dimExpressionPtrList.rend()))
    {
      //  get the lowerBound for each dimension
      SgExpression* dimLowerBound;
      SgExpression* newDimIndex;
      SgExpression* dimSize;
      /*  
        get the dimension size at each dimension
      */
      SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(*j2);
      /*
        This is for the 1st type of array declaration: a(10,15,20)
        Fortran is 1-based array.  Lowerbound is 1 by default.
      */
      if(subscriptExpression == NULL)
      {
        dimLowerBound = buildIntVal(1);
        dimSize = deepCopy(*j2);
      }
      /*
        This is for the 2nd type of array declaration: a(1:10,5:15,10:20)
        Actual dimension size = upperBound - lowerBound + 1
      */
      else
      {
        dimLowerBound = deepCopy(subscriptExpression->get_lowerBound());
        dimSize = buildAddOp(buildSubtractOp(deepCopy(subscriptExpression->get_upperBound()),
                                             deepCopy(subscriptExpression->get_lowerBound())),
                                             buildIntVal(1));
      }

      // convert the 1-based subscript to 0-based subscript
      newDimIndex = buildSubtractOp(deepCopy(*j1), dimLowerBound);
      if(j1 != subscriptExprList.rbegin())
      {
        newSubscript = buildAddOp(newDimIndex,
                                  buildMultiplyOp(dimSize,newSubscript));
      }
      else
      {
        newSubscript = newDimIndex;
        delete(dimSize);
      }
      ++j1;
      ++j2;
    } // end of while loop

    newSubscriptExprList.push_back(newSubscript);
    SgExprListExp* newSubscriptList = buildExprListExp(newSubscriptExprList);
    // un-link and remove the rhs operand
    pntrArrRefExp->get_rhs_operand()->set_parent(NULL);
    deepDelete(pntrArrRefExp->get_rhs_operand());
    // add the new subscriptExpression into rhs operand
    pntrArrRefExp->set_rhs_operand(newSubscriptList);
    newSubscriptList->set_parent(pntrArrRefExp);
    
  } // end of arraySubscript != NULL
}

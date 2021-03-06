#include <backstroke.h>
#include <pluggableReverser/eventProcessor.h>
#include <pluggableReverser/expAndStmtHandlers.h>
#include <utilities/utilities.h>
#include <normalizations/expNormalization.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <boost/timer.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

//The files are included from the generated code. We do this so they are used by some source file
#if 0
#include "test/rctypes.h"
#endif

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;
using namespace BackstrokeUtility;

bool isEvent(SgFunctionDeclaration* func)
{
	string func_name = func->get_name();
	if (starts_with(func_name, "event") &&
			!ends_with(func_name, "reverse") &&
			!ends_with(func_name, "forward"))
		return true;
	return false;
}

void addHandlers(EventProcessor& event_processor)
{
	// Add all expression handlers to the expression pool.
	//event_processor.addExpressionHandler(new NullExpressionHandler);
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);
	//event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);

	// Add all statement handlers to the statement pool.
	event_processor.addStatementHandler(new SgVariableDeclaration_Handler);
	event_processor.addStatementHandler(new SgBasicBlock_Handler);
	event_processor.addStatementHandler(new SgIfStmt_Handler);
	event_processor.addStatementHandler(new SgWhileStmt_Handler);
	event_processor.addStatementHandler(new SgReturnStmt_Handler);
	event_processor.addStatementHandler(new StateSavingStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);
}

void reverseEvents(SgProject* project)
{
	EventProcessor event_processor;
	addHandlers(event_processor);

	Backstroke::reverseEvents(&event_processor, isEvent, project);
}


int main(int argc, char * argv[])
{
	SgProject* project = frontend(argc, argv);
	reverseEvents(project);
	return backend(project);
}

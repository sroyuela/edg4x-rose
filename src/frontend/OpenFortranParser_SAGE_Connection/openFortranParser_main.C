/**
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract DE-
 * AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
 * operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *  
 * Additionally, this program and the accompanying materials are made
 * available under the terms of the Eclipse Public License v1.0 which
 * accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */

/* Based on examples/docs from:
 *      http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/invocation.html#wp9502
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/jniTOC.html
 * http://java.sun.com/docs/books/jni/html/invoke.html 
 */
#include "sage3basic.h"

#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "rose_config.h"
#include "rose_paths.h"
#include "commandline_processing.h"
#include "assert.h"
#include "fortran_error_handler.h"
#include "ofp.h"

/* This is defined if ROSE is configured to use the Java Open Fortran Parser */
#ifndef USE_ROSE_JAVA_SUPPORT
#error "openFortranParser_main.C should not be compiled when Java support is disabled"
#endif


using namespace std;

// DQ (9/6/2010): Allow this to be commented out to simplify debugging using gdb.
// #define ENABLE_FORTRAN_ERROR_HANDLER


/* This is critical to permitting the correct library (the one just built by the user)
// to be used instead of requiring the LD_LIBRARY_PATH to beexplicitly set by the user
// before running any tests.  It is alos important to proper testing using "make distcheck"
// since we want the new library built using that rule to be tested over any other.
*/
#define OVERWRITE_LD_LIBRARY_PATH 1


int openFortranParser_main(int argc, char **argv)
  {
 /* To use different versions of the LD_LIBRARY_PATH, get the value, 
    change it to the path in the build tree, and then reset it to the 
    old value after the call to JVM.  We need to do this because the
    libparser_java_FortranParserActionJNI.so that we need is in the 
    build tree and it is specific to the configuration of ROSE (else 
    V_<class name> enum values will be different as a result of 
    configuration options that might trigger different numbers of IR 
    nodes to be use.
  */

 /* Overwite to a new value. It is not clear when to use the install path and when to use the build path! */
    string new_value = findRoseSupportPathFromBuild("src/frontend/OpenFortranParser_SAGE_Connection/.libs", "lib");

 /* Save the old value */
    const char* old_value = getenv(ROSE_SHLIBPATH_VAR); // Might be null

#if OVERWRITE_LD_LIBRARY_PATH
    int overwrite = 1;
    int env_status = setenv(ROSE_SHLIBPATH_VAR,new_value.c_str(),overwrite);
    assert(env_status == 0);
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("Call the function that will start a JVM and call the OFP \n\n");
         string JVM_command_line = CommandlineProcessing::generateStringFromArgList(CommandlineProcessing::generateArgListFromArgcArgv(argc, argv));
         printf ("Java JVM commandline = %s \n",JVM_command_line.c_str());
         printf ("ROSE modified %s = %s \n",ROSE_SHLIBPATH_VAR,new_value.c_str());
       }

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_begin();
#endif

    int status = jvm_ofp_processing(argc, argv);

#ifdef ENABLE_FORTRAN_ERROR_HANDLER
    fortran_error_handler_end();
#endif

    if (SgProject::get_verbose() > 0)
       {
         printf ("JVM processing done.\n\n");
       }

 /* Reset to the saved value */
#if OVERWRITE_LD_LIBRARY_PATH
 // DQ (9/12/2011): Note that old_value can be NULL and if so then we don't want it to be dereferenced.
 // env_status = setenv(ROSE_SHLIBPATH_VAR,old_value,overwrite);
    if (old_value != NULL)
          env_status = setenv(ROSE_SHLIBPATH_VAR,old_value,overwrite);

    assert(env_status == 0);
#endif

         return status;
  }



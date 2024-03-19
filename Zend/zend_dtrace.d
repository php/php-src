/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies USA, Inc. (https://www.zend.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.01 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.zend.com/sites/zend/files/pdfs/zend-engine-license-2.01.txt |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: David Soria Parra <david.soriaparra@sun.com>                |
   +----------------------------------------------------------------------+
*/

provider php {
	probe exception__caught(char *classname);
	probe exception__thrown(char* classname);
	probe request__startup(char* request_file, char* request_uri, char* request_method);
	probe request__shutdown(char* request_file, char* request_uri, char* request_method);
	probe compile__file__entry(char * compile_file, char *compile_file_translated);
	probe compile__file__return(char *compile_file, char *compile_file_translated);
	probe error(char *errormsg, char *request_file, int lineno);
	probe execute__entry(char* request_file, int lineno);
	probe execute__return(char* request_file, int lineno);
	probe function__entry(char* function_name, char* request_file, int lineno, char* classname, char* scope);
	probe function__return(char* function_name, char* request_file, int lineno, char* classname, char* scope);
};

/*#pragma D attributes Private/Private/Unknown provider php module
#pragma D attributes Private/Private/Unknown provider php function
#pragma D attributes Evolving/Evolving/Common provider php provider */

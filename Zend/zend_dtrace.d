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

#pragma D attributes Evolving/Evolving/Common provider php provider
#pragma D attributes Private/Private/Unknown provider php module
#pragma D attributes Private/Private/Unknown provider php function

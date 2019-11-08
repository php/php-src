/* This is a generated file, edit the .stub.php file instead. */

#define PARSE_PARAMETERS_SET_TIME_LIMIT(seconds) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(seconds) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_OB_FLUSH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_CLEAN() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_END_FLUSH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_END_CLEAN() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_FLUSH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_CLEAN() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_CONTENTS() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_LEVEL() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_LENGTH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_LIST_HANDLERS() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OB_GET_STATUS(full_status) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(full_status) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_OB_IMPLICIT_FLUSH(flag) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(flag) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_OUTPUT_RESET_REWRITE_VARS() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_OUTPUT_ADD_REWRITE_VAR(name, name_length, value, value_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(name, name_length) \
		Z_PARAM_STRING(value, value_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STREAM_WRAPPER_REGISTER(protocol, protocol_length, classname, classname_length, flags) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(protocol, protocol_length) \
		Z_PARAM_STRING(classname, classname_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(flags) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STREAM_WRAPPER_UNREGISTER(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STREAM_WRAPPER_RESTORE(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_SLICE(arg, offset, length, preserve_keys) \
	ZEND_PARSE_PARAMETERS_START(2, 4) \
		Z_PARAM_ARRAY(arg) \
		Z_PARAM_LONG(offset) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG_OR_NULL(length) \
		Z_PARAM_BOOL(preserve_keys) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_KEY_FIRST(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_KEY_LAST(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_VALUES(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_COUNT_VALUES(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_REVERSE(input, preserve_keys) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_ARRAY(input) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(preserve_keys) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_FLIP(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_CHANGE_KEY_CASE(input, case) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_ARRAY(input) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(case) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_UNIQUE(arg, flags) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_ARRAY(arg) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(flags) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_RAND(arg, num_req) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_ARRAY(arg) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(num_req) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_SUM(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_PRODUCT(arg) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_ARRAY(arg) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_FILTER(arg, callback, use_keys) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_ARRAY(arg) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_FUNCT(callback) \
		Z_PARAM_LONG(use_keys) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_CHUNK(arg, size, preserve_keys) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_ARRAY(arg) \
		Z_PARAM_LONG(size) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(preserve_keys) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ARRAY_COMBINE(keys, values) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_ARRAY(keys) \
		Z_PARAM_ARRAY(values) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_BASE64_ENCODE(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_BASE64_DECODE(str, str_length, strict) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(strict) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CONSTANT(name, name_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(name, name_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IP2LONG(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LONG2IP(proper_address) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(proper_address) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GETENV(variable, variable_length, local_only) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(variable, variable_length) \
		Z_PARAM_BOOL(local_only) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_PUTENV)
#define PARSE_PARAMETERS_PUTENV(setting, setting_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(setting, setting_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_FLUSH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_SLEEP(seconds) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(seconds) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_USLEEP(microseconds) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(microseconds) \
	ZEND_PARSE_PARAMETERS_END();

#if HAVE_NANOSLEEP
#define PARSE_PARAMETERS_NANOSLEEP(seconds, nanoseconds) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(seconds) \
		Z_PARAM_LONG(nanoseconds) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if HAVE_NANOSLEEP
#define PARSE_PARAMETERS_TIME_SLEEP_UNTIL(timestamp) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(timestamp) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_GET_CURRENT_USER() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GET_CFG_VAR(option_name, option_name_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(option_name, option_name_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GET_MAGIC_QUOTES_RUNTIME() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GET_MAGIC_QUOTES_GPC() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_ERROR_LOG(message, message_length, message_type, destination, destination_length, extra_headers, extra_headers_length) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_STRING(message, message_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(message_type) \
		Z_PARAM_STRING(destination, destination_length) \
		Z_PARAM_STRING(extra_headers, extra_headers_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ERROR_GET_LAST() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_ERROR_CLEAR_LAST() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_CALL_USER_FUNC_ARRAY(function, args) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_FUNCT(function) \
		Z_PARAM_ARRAY(args) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FORWARD_STATIC_CALL_ARRAY(function, args) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_FUNCT(function) \
		Z_PARAM_ARRAY(args) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HIGHLIGHT_FILE(filename, filename_length, return) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(return) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PHP_STRIP_WHITESPACE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HIGHLIGHT_STRING(string, string_length, return) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(return) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_INI_GET(varname, varname_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(varname, varname_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_INI_GET_ALL(extension, extension_length, details) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING_OR_NULL(extension, extension_length) \
		Z_PARAM_BOOL(details) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_INI_SET(varname, varname_length, value, value_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(varname, varname_length) \
		Z_PARAM_STRING(value, value_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_INI_RESTORE(varname, varname_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(varname, varname_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SET_INCLUDE_PATH(include_path, include_path_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(include_path, include_path_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GET_INCLUDE_PATH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_RESTORE_INCLUDE_PATH() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_CONNECTION_ABORTED() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_CONNECTION_STATUS() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_IGNORE_USER_ABORT(value) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(value) \
	ZEND_PARSE_PARAMETERS_END();

#if HAVE_GETSERVBYNAME
#define PARSE_PARAMETERS_GETSERVBYNAME(service, service_length, protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(service, service_length) \
		Z_PARAM_STRING(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if HAVE_GETSERVBYPORT
#define PARSE_PARAMETERS_GETSERVBYPORT(port, protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(port) \
		Z_PARAM_STRING(protocol, protocol_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if HAVE_GETPROTOBYNAME
#define PARSE_PARAMETERS_GETPROTOBYNAME(name, name_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(name, name_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if HAVE_GETPROTOBYNUMBER
#define PARSE_PARAMETERS_GETPROTOBYNUMBER(protocol) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(protocol) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_IS_UPLOADED_FILE(path, path_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(path, path_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_MOVE_UPLOADED_FILE(path, path_length, new_path, new_path_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(path, path_length) \
		Z_PARAM_STRING(new_path, new_path_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PARSE_INI_FILE(filename, filename_length, process_sections, scanner_mode) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(process_sections) \
		Z_PARAM_LONG(scanner_mode) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PARSE_INI_STRING(ini_string, ini_string_length, process_sections, scanner_mode) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(ini_string, ini_string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(process_sections) \
		Z_PARAM_LONG(scanner_mode) \
	ZEND_PARSE_PARAMETERS_END();

#if ZEND_DEBUG
#define PARSE_PARAMETERS_CONFIG_GET_HASH(ini_string, ini_string_length, process_sections, scanner_mode) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(ini_string, ini_string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(process_sections) \
		Z_PARAM_LONG(scanner_mode) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_GETLOADAVG)
#define PARSE_PARAMETERS_SYS_GETLOADAVG() \
	ZEND_PARSE_PARAMETERS_NONE();
#endif

#define PARSE_PARAMETERS_GET_BROWSER(browser_name, browser_name_length, return_array) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING_OR_NULL(browser_name, browser_name_length) \
		Z_PARAM_BOOL(return_array) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CRC32(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CRYPT(str, str_length, salt, salt_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(salt, salt_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CONVERT_CYR_STRING(str, str_length, from, from_length, to, to_length) \
	ZEND_PARSE_PARAMETERS_START(3, 3) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_STRING(from, from_length) \
		Z_PARAM_STRING(to, to_length) \
	ZEND_PARSE_PARAMETERS_END();

#if HAVE_STRPTIME
#define PARSE_PARAMETERS_STRPTIME(timestamp, timestamp_length, format, format_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(timestamp, timestamp_length) \
		Z_PARAM_STRING(format, format_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_GETHOSTNAME)
#define PARSE_PARAMETERS_GETHOSTNAME() \
	ZEND_PARSE_PARAMETERS_NONE();
#endif

#define PARSE_PARAMETERS_GETHOSTBYADDR(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GETHOSTBYNAME(hostname, hostname_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(hostname, hostname_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GETHOSTBYNAMEL(hostname, hostname_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(hostname, hostname_length) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
#define PARSE_PARAMETERS_DNS_CHECK_RECORD(hostname, hostname_length, type, type_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(hostname, hostname_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(type, type_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_NET_GET_INTERFACES() \
	ZEND_PARSE_PARAMETERS_NONE();

#if HAVE_FTOK
#define PARSE_PARAMETERS_FTOK(pathname, pathname_length, proj, proj_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(pathname, pathname_length) \
		Z_PARAM_STRING(proj, proj_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_HRTIME(get_as_number) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(get_as_number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LCG_VALUE() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_MD5(str, str_length, raw_output) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(raw_output) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_MD5_FILE(filename, filename_length, raw_output) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(raw_output) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GETMYUID() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GETMYGID() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GETMYPID() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GETMYINODE() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_GETLASTMOD() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_SHA1(str, str_length, raw_output) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(raw_output) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SHA1_FILE(filename, filename_length, raw_output) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(raw_output) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_SYSLOG_H)
#define PARSE_PARAMETERS_OPENLOG(ident, ident_length, option, facility) \
	ZEND_PARSE_PARAMETERS_START(3, 3) \
		Z_PARAM_STRING(ident, ident_length) \
		Z_PARAM_LONG(option) \
		Z_PARAM_LONG(facility) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_SYSLOG_H)
#define PARSE_PARAMETERS_CLOSELOG() \
	ZEND_PARSE_PARAMETERS_NONE();
#endif

#if defined(HAVE_SYSLOG_H)
#define PARSE_PARAMETERS_SYSLOG(priority, message, message_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(priority) \
		Z_PARAM_STRING(message, message_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_INET_NTOP)
#define PARSE_PARAMETERS_INET_NTOP(in_addr, in_addr_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(in_addr, in_addr_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_INET_PTON)
#define PARSE_PARAMETERS_INET_PTON(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(ip_address, ip_address_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_METAPHONE(text, text_length, phones) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(text, text_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(phones) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEADER(string, string_length, replace, http_response_code) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(replace) \
		Z_PARAM_LONG(http_response_code) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEADER_REMOVE(name, name_length) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(name, name_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HTTP_RESPONSE_CODE(response_code) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(response_code) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEADERS_LIST() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_HTMLSPECIALCHARS(string, string_length, quote_style, encoding, encoding_length, double_encode) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(quote_style) \
		Z_PARAM_STRING_OR_NULL(encoding, encoding_length) \
		Z_PARAM_BOOL(double_encode) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HTMLSPECIALCHARS_DECODE(string, string_length, quote_style) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(quote_style) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HTML_ENTITY_DECODE(string, string_length, quote_style, encoding, encoding_length) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(quote_style) \
		Z_PARAM_STRING(encoding, encoding_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HTMLENTITIES(string, string_length, quote_style, encoding, encoding_length, double_encode) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(quote_style) \
		Z_PARAM_STRING_OR_NULL(encoding, encoding_length) \
		Z_PARAM_BOOL(double_encode) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_GET_HTML_TRANSLATION_TABLE(table, quote_style, encoding, encoding_length) \
	ZEND_PARSE_PARAMETERS_START(0, 3) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(table) \
		Z_PARAM_LONG(quote_style) \
		Z_PARAM_STRING(encoding, encoding_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_BIN2HEX(data, data_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(data, data_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEX2BIN(data, data_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(data, data_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRSPN(str, str_length, mask, mask_length, start, len) \
	ZEND_PARSE_PARAMETERS_START(2, 4) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_STRING(mask, mask_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(start) \
		Z_PARAM_LONG(len) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRCSPN(str, str_length, mask, mask_length, start, len) \
	ZEND_PARSE_PARAMETERS_START(2, 4) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_STRING(mask, mask_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(start) \
		Z_PARAM_LONG(len) \
	ZEND_PARSE_PARAMETERS_END();

#if HAVE_NL_LANGINFO
#define PARSE_PARAMETERS_NL_LANGINFO(item) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(item) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_STRCOLL(str1, str1_length, str2, str2_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(str1, str1_length) \
		Z_PARAM_STRING(str2, str2_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_TRIM(str, str_length, character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_RTRIM(str, str_length, character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LTRIM(str, str_length, character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(character_mask, character_mask_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_WORDWRAP(str, str_length, width, break, break_length, cut) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(width) \
		Z_PARAM_STRING(break, break_length) \
		Z_PARAM_BOOL(cut) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_EXPLODE(separator, separator_length, str, str_length, limit) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(separator, separator_length) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(limit) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRTOK(str, str_length, token, token_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(token, token_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRTOUPPER(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRTOLOWER(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_BASENAME(path, path_length, suffix, suffix_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(path, path_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(suffix, suffix_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_DIRNAME(path, path_length, levels) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(path, path_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(levels) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PATHINFO(path, path_length, options) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(path, path_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(options) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRISTR(haystack, haystack_length, needle, needle_length, before_needle) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(before_needle) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRSTR(haystack, haystack_length, needle, needle_length, before_needle) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(before_needle) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRPOS(haystack, haystack_length, needle, needle_length, offset) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRIPOS(haystack, haystack_length, needle, needle_length, offset) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRRPOS(haystack, haystack_length, needle, needle_length, offset) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRRIPOS(haystack, haystack_length, needle, needle_length, offset) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRRCHR(haystack, haystack_length, needle, needle_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CHUNK_SPLIT(str, str_length, chunklen, ending, ending_length) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(chunklen) \
		Z_PARAM_STRING(ending, ending_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SUBSTR(str, str_length, start, length) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_LONG(start) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG_OR_NULL(length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_QUOTEMETA(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ORD(character, character_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(character, character_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CHR(codepoint) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(codepoint) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_UCFIRST(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LCFIRST(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_UCWORDS(str, str_length, delimiters, delimiters_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(delimiters, delimiters_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRREV(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ADDCSLASHES(str, str_length, charlist, charlist_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_STRING(charlist, charlist_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ADDSLASHES(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRIPCSLASHES(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRIPSLASHES(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEBREV(str, str_length, max_chars_per_line) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(max_chars_per_line) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEBREVC(str, str_length, max_chars_per_line) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(max_chars_per_line) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_NL2BR(str, str_length, is_xhtml) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(is_xhtml) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_GETCSV(string, string_length, delimiter, delimiter_length, enclosure, enclosure_length, escape, escape_length) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_STRING(string, string_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(delimiter, delimiter_length) \
		Z_PARAM_STRING(enclosure, enclosure_length) \
		Z_PARAM_STRING(escape, escape_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_REPEAT(input, input_length, mult) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(input, input_length) \
		Z_PARAM_LONG(mult) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_COUNT_CHARS(input, input_length, mode) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(input, input_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(mode) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRNATCMP(s1, s1_length, s2, s2_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(s1, s1_length) \
		Z_PARAM_STRING(s2, s2_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LOCALECONV() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_STRNATCASECMP(s1, s1_length, s2, s2_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(s1, s1_length) \
		Z_PARAM_STRING(s2, s2_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SUBSTR_COUNT(haystack, haystack_length, needle, needle_length, offset, length) \
	ZEND_PARSE_PARAMETERS_START(2, 4) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(needle, needle_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
		Z_PARAM_LONG_OR_NULL(length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_PAD(input, input_length, pad_length, pad_string, pad_string_length, pad_type) \
	ZEND_PARSE_PARAMETERS_START(2, 4) \
		Z_PARAM_STRING(input, input_length) \
		Z_PARAM_LONG(pad_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(pad_string, pad_string_length) \
		Z_PARAM_LONG(pad_type) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_ROT13(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_SHUFFLE(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STR_WORD_COUNT(str, str_length, format, charlist, charlist_length) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(format) \
		Z_PARAM_STRING(charlist, charlist_length) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_STRFMON)
#define PARSE_PARAMETERS_MONEY_FORMAT(format, format_length, value) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(format, format_length) \
		Z_PARAM_DOUBLE(value) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_STR_SPLIT(str, str_length, split_length) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(split_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STRPBRK(haystack, haystack_length, char_list, char_list_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(haystack, haystack_length) \
		Z_PARAM_STRING(char_list, char_list_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SUBSTR_COMPARE(main_str, main_str_length, str, str_length, offset, length, case_insensitivity) \
	ZEND_PARSE_PARAMETERS_START(3, 5) \
		Z_PARAM_STRING(main_str, main_str_length) \
		Z_PARAM_STRING(str, str_length) \
		Z_PARAM_LONG(offset) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG_OR_NULL(length) \
		Z_PARAM_BOOL(case_insensitivity) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_UTF8_ENCODE(data, data_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(data, data_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_UTF8_DECODE(data, data_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(data, data_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CHDIR(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
#define PARSE_PARAMETERS_CHROOT(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_GETCWD() \
	ZEND_PARSE_PARAMETERS_NONE();

#if defined(HAVE_GLOB)
#define PARSE_PARAMETERS_GLOB(pattern, pattern_length, flags) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_STRING(pattern, pattern_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(flags) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_FILEATIME(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILECTIME(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILEGROUP(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILEINODE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILEMTIME(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILEOWNER(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILEPERMS(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILESIZE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILETYPE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FILE_EXISTS(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_WRITABLE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_WRITEABLE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_READABLE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_EXECUTABLE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_FILE(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_DIR(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_LINK(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_STAT(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LSTAT(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CHMOD(filename, filename_length, mode) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_LONG(mode) \
	ZEND_PARSE_PARAMETERS_END();

#if HAVE_UTIME
#define PARSE_PARAMETERS_TOUCH(filename, filename_length, time, atime) \
	ZEND_PARSE_PARAMETERS_START(1, 3) \
		Z_PARAM_STRING(filename, filename_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(time) \
		Z_PARAM_LONG(atime) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_CLEARSTATCACHE(clear_realpath_cache, filename, filename_length) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(clear_realpath_cache) \
		Z_PARAM_STRING(filename, filename_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_DISK_TOTAL_SPACE(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_DISK_FREE_SPACE(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_DISKFREESPACE(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(directory, directory_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_REALPATH_CACHE_GET() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_REALPATH_CACHE_SIZE() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_PHPINFO(what) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(what) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PHPVERSION(extension, extension_length) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(extension, extension_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PHPCREDITS(flag) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(flag) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PHP_SAPI_NAME() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_PHP_UNAME(mode, mode_length) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_STRING(mode, mode_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PHP_INI_SCANNED_FILES() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_PHP_INI_LOADED_FILE() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_IPTCEMBED(iptcdata, iptcdata_length, jpeg_file_name, jpeg_file_name_length, spool) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(iptcdata, iptcdata_length) \
		Z_PARAM_STRING(jpeg_file_name, jpeg_file_name_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(spool) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IPTCPARSE(iptcblock, iptcblock_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(iptcblock, iptcblock_length) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
#define PARSE_PARAMETERS_READLINK(path, path_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(path, path_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
#define PARSE_PARAMETERS_LINKINFO(path, path_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(path, path_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
#define PARSE_PARAMETERS_SYMLINK(target, target_length, link, link_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(target, target_length) \
		Z_PARAM_STRING(link, link_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
#define PARSE_PARAMETERS_LINK(target, target_length, link, link_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(target, target_length) \
		Z_PARAM_STRING(link, link_length) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_EZMLM_HASH(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_CEIL(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FLOOR(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SIN(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_COS(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_TAN(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ASIN(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ACOS(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ATAN(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ATANH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ATAN2(y, x) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_DOUBLE(y) \
		Z_PARAM_DOUBLE(x) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SINH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_COSH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_TANH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ASINH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_ACOSH(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_EXPM1(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LOG1P(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PI() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_IS_FINITE(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_NAN(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_INTDIV(dividend, divisor) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(dividend) \
		Z_PARAM_LONG(divisor) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_IS_INFINITE(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_EXP(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LOG(number, base) \
	ZEND_PARSE_PARAMETERS_START(1, 2) \
		Z_PARAM_DOUBLE(number) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_DOUBLE(base) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_LOG10(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_SQRT(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HYPOT(x, y) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_DOUBLE(x) \
		Z_PARAM_DOUBLE(y) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_DEG2RAD(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_RAD2DEG(number) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_DOUBLE(number) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_BINDEC(binary_string, binary_string_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(binary_string, binary_string_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_HEXDEC(hex_string, hex_string_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(hex_string, hex_string_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_OCTDEC(octal_string, octal_string_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(octal_string, octal_string_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_NUMBER_FORMAT(number, decimals, decimal_point, decimal_point_length, thousands_separator, thousands_separator_length) \
	ZEND_PARSE_PARAMETERS_START(1, 4) \
		Z_PARAM_DOUBLE(number) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(decimals) \
		Z_PARAM_STRING_OR_NULL(decimal_point, decimal_point_length) \
		Z_PARAM_STRING_OR_NULL(thousands_separator, thousands_separator_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FMOD(x, y) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_DOUBLE(x) \
		Z_PARAM_DOUBLE(y) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_FDIV(dividend, divisor) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_DOUBLE(dividend) \
		Z_PARAM_DOUBLE(divisor) \
	ZEND_PARSE_PARAMETERS_END();

#if defined(HAVE_GETTIMEOFDAY)
#define PARSE_PARAMETERS_MICROTIME(get_as_float) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(get_as_float) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_GETTIMEOFDAY)
#define PARSE_PARAMETERS_GETTIMEOFDAY(return_float) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_BOOL(return_float) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#if defined(HAVE_GETRUSAGE)
#define PARSE_PARAMETERS_GETRUSAGE(who) \
	ZEND_PARSE_PARAMETERS_START(0, 1) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(who) \
	ZEND_PARSE_PARAMETERS_END();
#endif

#define PARSE_PARAMETERS_UNPACK(format, format_length, data, data_length, offset) \
	ZEND_PARSE_PARAMETERS_START(2, 3) \
		Z_PARAM_STRING(format, format_length) \
		Z_PARAM_STRING(data, data_length) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(offset) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PASSWORD_GET_INFO(hash, hash_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(hash, hash_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PASSWORD_VERIFY(password, password_length, hash, hash_length) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_STRING(password, password_length) \
		Z_PARAM_STRING(hash, hash_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_PASSWORD_ALGOS() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_QUOTED_PRINTABLE_DECODE(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_QUOTED_PRINTABLE_ENCODE(str, str_length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_STRING(str, str_length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_MT_SRAND(seed, mode) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(seed) \
		Z_PARAM_LONG(mode) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_MT_RAND(min, max) \
	ZEND_PARSE_PARAMETERS_START(0, 2) \
		Z_PARAM_OPTIONAL \
		Z_PARAM_LONG(min) \
		Z_PARAM_LONG(max) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_MT_GETRANDMAX() \
	ZEND_PARSE_PARAMETERS_NONE();

#define PARSE_PARAMETERS_RANDOM_BYTES(length) \
	ZEND_PARSE_PARAMETERS_START(1, 1) \
		Z_PARAM_LONG(length) \
	ZEND_PARSE_PARAMETERS_END();

#define PARSE_PARAMETERS_RANDOM_INT(min, max) \
	ZEND_PARSE_PARAMETERS_START(2, 2) \
		Z_PARAM_LONG(min) \
		Z_PARAM_LONG(max) \
	ZEND_PARSE_PARAMETERS_END();

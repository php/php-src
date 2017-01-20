/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Colin Viebrock <colin@viebrock.ca>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "ext/standard/html.h"
#include "info.h"
#include "credits.h"
#include "css.h"
#include "SAPI.h"
#include <time.h>
#include "php_main.h"
#include "zend_globals.h"		/* needs ELS */
#include "zend_extensions.h"
#include "zend_highlight.h"
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#include "url.h"
#include "php_string.h"

#ifdef PHP_WIN32
typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
# include "winver.h"

#endif

#define SECTION(name)	if (!sapi_module.phpinfo_as_text) { \
							php_info_print("<h2>" name "</h2>\n"); \
						} else { \
							php_info_print_table_start(); \
							php_info_print_table_header(1, name); \
							php_info_print_table_end(); \
						} \

PHPAPI extern char *php_ini_opened_path;
PHPAPI extern char *php_ini_scanned_path;
PHPAPI extern char *php_ini_scanned_files;

static int php_info_print_html_esc(const char *str, size_t len) /* {{{ */
{
	size_t written;
	zend_string *new_str;

	new_str = php_escape_html_entities((unsigned char *) str, len, 0, ENT_QUOTES, "utf-8");
	written = php_output_write(ZSTR_VAL(new_str), ZSTR_LEN(new_str));
	zend_string_free(new_str);
	return written;
}
/* }}} */

static int php_info_printf(const char *fmt, ...) /* {{{ */
{
	char *buf;
	size_t len, written;
	va_list argv;

	va_start(argv, fmt);
	len = vspprintf(&buf, 0, fmt, argv);
	va_end(argv);

	written = php_output_write(buf, len);
	efree(buf);
	return written;
}
/* }}} */

static int php_info_print(const char *str) /* {{{ */
{
	return php_output_write(str, strlen(str));
}
/* }}} */

static void php_info_print_stream_hash(const char *name, HashTable *ht) /* {{{ */
{
	zend_string *key;

	if (ht) {
		if (zend_hash_num_elements(ht)) {
			int first = 1;

			if (!sapi_module.phpinfo_as_text) {
				php_info_printf("<tr><td class=\"e\">Registered %s</td><td class=\"v\">", name);
			} else {
				php_info_printf("\nRegistered %s => ", name);
			}

			ZEND_HASH_FOREACH_STR_KEY(ht, key) {
				if (key) {
					if (first) {
						first = 0;
					} else {
						php_info_print(", ");
					}
					if (!sapi_module.phpinfo_as_text) {
						php_info_print_html_esc(ZSTR_VAL(key), ZSTR_LEN(key));
					} else {
						php_info_print(ZSTR_VAL(key));
					}
				}
			} ZEND_HASH_FOREACH_END();

			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td></tr>\n");
			}
		} else {
			char reg_name[128];
			snprintf(reg_name, sizeof(reg_name), "Registered %s", name);
			php_info_print_table_row(2, reg_name, "none registered");
		}
	} else {
		php_info_print_table_row(2, name, "disabled");
	}
}
/* }}} */

PHPAPI void php_info_print_module(zend_module_entry *zend_module) /* {{{ */
{
	if (zend_module->info_func || zend_module->version) {
		if (!sapi_module.phpinfo_as_text) {
			zend_string *url_name = php_url_encode(zend_module->name, strlen(zend_module->name));

			php_strtolower(ZSTR_VAL(url_name), ZSTR_LEN(url_name));
			php_info_printf("<h2><a name=\"module_%s\">%s</a></h2>\n", ZSTR_VAL(url_name), zend_module->name);

			efree(url_name);
		} else {
			php_info_print_table_start();
			php_info_print_table_header(1, zend_module->name);
			php_info_print_table_end();
		}
		if (zend_module->info_func) {
			zend_module->info_func(zend_module);
		} else {
			php_info_print_table_start();
			php_info_print_table_row(2, "Version", zend_module->version);
			php_info_print_table_end();
			DISPLAY_INI_ENTRIES();
		}
	} else {
		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<tr><td class=\"v\">%s</td></tr>\n", zend_module->name);
		} else {
			php_info_printf("%s\n", zend_module->name);
		}
	}
}
/* }}} */

static int _display_module_info_func(zval *el) /* {{{ */
{
	zend_module_entry *module = (zend_module_entry*)Z_PTR_P(el);
	if (module->info_func || module->version) {
		php_info_print_module(module);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int _display_module_info_def(zval *el) /* {{{ */
{
	zend_module_entry *module = (zend_module_entry*)Z_PTR_P(el);
	if (!module->info_func && !module->version) {
		php_info_print_module(module);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ php_print_gpcse_array
 */
static void php_print_gpcse_array(char *name, uint name_length)
{
	zval *data, *tmp, tmp2;
	zend_string *string_key;
	zend_ulong num_key;
	zend_string *key;

	key = zend_string_init(name, name_length, 0);
	zend_is_auto_global(key);

	if ((data = zend_hash_find(&EG(symbol_table), key)) != NULL && (Z_TYPE_P(data) == IS_ARRAY)) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), num_key, string_key, tmp) {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("<tr>");
				php_info_print("<td class=\"e\">");
			}

			php_info_print("$");
			php_info_print(name);
			php_info_print("['");

			if (string_key != NULL) {
				if (!sapi_module.phpinfo_as_text) {
					php_info_print_html_esc(ZSTR_VAL(string_key), ZSTR_LEN(string_key));
				} else {
					php_info_print(ZSTR_VAL(string_key));
				}
			} else {
				php_info_printf(ZEND_ULONG_FMT, num_key);
			}
			php_info_print("']");
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td><td class=\"v\">");
			} else {
				php_info_print(" => ");
			}
			if (Z_TYPE_P(tmp) == IS_ARRAY) {
				if (!sapi_module.phpinfo_as_text) {
					php_info_print("<pre>");
					zend_print_zval_r_ex((zend_write_func_t) php_info_print_html_esc, tmp, 0);
					php_info_print("</pre>");
				} else {
					zend_print_zval_r(tmp, 0);
				}
			} else {
				ZVAL_COPY_VALUE(&tmp2, tmp);
				if (Z_TYPE(tmp2) != IS_STRING) {
					tmp = NULL;
					zval_copy_ctor(&tmp2);
					convert_to_string(&tmp2);
				}

				if (!sapi_module.phpinfo_as_text) {
					if (Z_STRLEN(tmp2) == 0) {
						php_info_print("<i>no value</i>");
					} else {
						php_info_print_html_esc(Z_STRVAL(tmp2), Z_STRLEN(tmp2));
					}
				} else {
					php_info_print(Z_STRVAL(tmp2));
				}

				if (!tmp) {
					zval_dtor(&tmp2);
				}
			}
			if (!sapi_module.phpinfo_as_text) {
				php_info_print("</td></tr>\n");
			} else {
				php_info_print("\n");
			}
		} ZEND_HASH_FOREACH_END();
	}
	zend_string_free(key);
}
/* }}} */

/* {{{ php_info_print_style
 */
void php_info_print_style(void)
{
	php_info_printf("<style type=\"text/css\">\n");
	php_info_print_css();
	php_info_printf("</style>\n");
}
/* }}} */

/* {{{ php_info_html_esc
 */
PHPAPI zend_string *php_info_html_esc(char *string)
{
	return php_escape_html_entities((unsigned char *) string, strlen(string), 0, ENT_QUOTES, NULL);
}
/* }}} */

#ifdef PHP_WIN32
/* {{{  */

char* php_get_windows_name()
{
	OSVERSIONINFOEX osvi = EG(windows_version_info);
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	DWORD dwType;
	char *major = NULL, *sub = NULL, *retval;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));

	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if(NULL != pGNSI) {
		pGNSI(&si);
	} else {
		GetSystemInfo(&si);
	}

	if (VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion >= 10) {
		if (osvi.dwMajorVersion == 10) {
			if( osvi.dwMinorVersion == 0 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION ) {
					major = "Windows 10";
				} else {
					major = "Windows Server 2016";
				}
			}
		}
	} else if (VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion >= 6) {
		if (osvi.dwMajorVersion == 6) {
			if( osvi.dwMinorVersion == 0 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION ) {
					major = "Windows Vista";
				} else {
					major = "Windows Server 2008";
				}
			} else if ( osvi.dwMinorVersion == 1 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION )  {
					major = "Windows 7";
				} else {
					major = "Windows Server 2008 R2";
				}
			} else if ( osvi.dwMinorVersion == 2 ) {
				/* could be Windows 8/Windows Server 2012, could be Windows 8.1/Windows Server 2012 R2 */
				/* XXX and one more X - the above comment is true if no manifest is used for two cases:
					- if the PHP build doesn't use the correct manifest
					- if PHP DLL loaded under some binary that doesn't use the correct manifest 
					
					So keep the handling here as is for now, even if we know 6.2 is win8 and nothing else, and think about an improvement. */
				OSVERSIONINFOEX osvi81;
				DWORDLONG dwlConditionMask = 0;
				int op = VER_GREATER_EQUAL;

				ZeroMemory(&osvi81, sizeof(OSVERSIONINFOEX));
				osvi81.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
				osvi81.dwMajorVersion = 6;
				osvi81.dwMinorVersion = 3;
				osvi81.wServicePackMajor = 0;

				VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
				VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
				VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);

				if (VerifyVersionInfo(&osvi81,
					VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR,
					dwlConditionMask)) {
					osvi.dwMinorVersion = 3; /* Windows 8.1/Windows Server 2012 R2 */
					if( osvi.wProductType == VER_NT_WORKSTATION )  {
						major = "Windows 8.1";
					} else {
						major = "Windows Server 2012 R2";
					}
				} else {
					if( osvi.wProductType == VER_NT_WORKSTATION )  {
						major = "Windows 8";
					} else {
						major = "Windows Server 2012";
					}
				}
			} else if (osvi.dwMinorVersion == 3) {
				if( osvi.wProductType == VER_NT_WORKSTATION )  {
					major = "Windows 8.1";
				} else {
					major = "Windows Server 2012 R2";
				}
			} else {
				major = "Unknown Windows version";
			}

			pGPI = (PGPI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetProductInfo");
			pGPI(6, 0, 0, 0, &dwType);

			switch (dwType) {
				case PRODUCT_ULTIMATE:
					sub = "Ultimate Edition";
					break;
				case PRODUCT_HOME_BASIC:
					sub = "Home Basic Edition";
					break;
				case PRODUCT_HOME_PREMIUM:
					sub = "Home Premium Edition";
					break;
				case PRODUCT_ENTERPRISE:
					sub = "Enterprise Edition";
					break;
				case PRODUCT_HOME_BASIC_N:
					sub = "Home Basic N Edition";
					break;
				case PRODUCT_BUSINESS:
					if ((osvi.dwMajorVersion > 6) || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 0)) {
						sub = "Professional Edition";
					} else {
						sub = "Business Edition";
					}
					break;
				case PRODUCT_STANDARD_SERVER:
					sub = "Standard Edition";
					break;
				case PRODUCT_DATACENTER_SERVER:
					sub = "Datacenter Edition";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER:
					sub = "Small Business Server";
					break;
				case PRODUCT_ENTERPRISE_SERVER:
					sub = "Enterprise Edition";
					break;
				case PRODUCT_STARTER:
					if ((osvi.dwMajorVersion > 6) || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 0)) {
						sub = "Starter N Edition";
					} else {
					    sub = "Starter Edition";
					}
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					sub = "Datacenter Edition (core installation)";
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					sub = "Standard Edition (core installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					sub = "Enterprise Edition (core installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					sub = "Enterprise Edition for Itanium-based Systems";
					break;
				case PRODUCT_BUSINESS_N:
					if ((osvi.dwMajorVersion > 6) || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 0)) {
						sub = "Professional N Edition";
					} else {
						sub = "Business N Edition";
					}
					break;
				case PRODUCT_WEB_SERVER:
					sub = "Web Server Edition";
					break;
				case PRODUCT_CLUSTER_SERVER:
					sub = "HPC Edition";
					break;
				case PRODUCT_HOME_SERVER:
					sub = "Storage Server Essentials Edition";
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER:
					sub = "Storage Server Express Edition";
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER:
					sub = "Storage Server Standard Edition";
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER:
					sub = "Storage Server Workgroup Edition";
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER:
					sub = "Storage Server Enterprise Edition";
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS:
					sub = "Essential Server Solutions Edition";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
					sub = "Small Business Server Premium Edition";
					break;
				case PRODUCT_HOME_PREMIUM_N:
					sub = "Home Premium N Edition";
					break;
				case PRODUCT_ENTERPRISE_N:
					sub = "Enterprise N Edition";
					break;
				case PRODUCT_ULTIMATE_N:
					sub = "Ultimate N Edition";
					break;
				case PRODUCT_WEB_SERVER_CORE:
					sub = "Web Server Edition (core installation)";
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
					sub = "Essential Business Server Management Server Edition";
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
					sub = "Essential Business Server Management Security Edition";
					break;
				case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
					sub = "Essential Business Server Management Messaging Edition";
					break;
				case PRODUCT_SERVER_FOUNDATION:
					sub = "Foundation Edition";
					break;
				case PRODUCT_HOME_PREMIUM_SERVER:
					sub = "Home Server 2011 Edition";
					break;
				case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
					sub = "Essential Server Solutions Edition (without Hyper-V)";
					break;
				case PRODUCT_STANDARD_SERVER_V:
					sub = "Standard Edition (without Hyper-V)";
					break;
				case PRODUCT_DATACENTER_SERVER_V:
					sub = "Datacenter Edition (without Hyper-V)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_V:
					sub = "Enterprise Edition (without Hyper-V)";
					break;
				case PRODUCT_DATACENTER_SERVER_CORE_V:
					sub = "Datacenter Edition (core installation, without Hyper-V)";
					break;
				case PRODUCT_STANDARD_SERVER_CORE_V:
					sub = "Standard Edition (core installation, without Hyper-V)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE_V:
					sub = "Enterprise Edition (core installation, without Hyper-V)";
					break;
				case PRODUCT_HYPERV:
					sub = "Hyper-V Server";
					break;
				case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
					sub = "Storage Server Express Edition (core installation)";
					break;
				case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
					sub = "Storage Server Standard Edition (core installation)";
					break;
				case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
					sub = "Storage Server Workgroup Edition (core installation)";
					break;
				case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
					sub = "Storage Server Enterprise Edition (core installation)";
					break;
				case PRODUCT_STARTER_N:
					sub = "Starter N Edition";
					break;
				case PRODUCT_PROFESSIONAL:
					sub = "Professional Edition";
					break;
				case PRODUCT_PROFESSIONAL_N:
					sub = "Professional N Edition";
					break;
				case PRODUCT_SB_SOLUTION_SERVER:
					sub = "Small Business Server 2011 Essentials Edition";
					break;
				case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
					sub = "Server For SB Solutions Edition";
					break;
				case PRODUCT_STANDARD_SERVER_SOLUTIONS:
					sub = "Solutions Premium Edition";
					break;
				case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
					sub = "Solutions Premium Edition (core installation)";
					break;
				case PRODUCT_SB_SOLUTION_SERVER_EM:
					sub = "Server For SB Solutions EM Edition";
					break;
				case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
					sub = "Server For SB Solutions EM Edition";
					break;
				case PRODUCT_SOLUTION_EMBEDDEDSERVER:
					sub = "MultiPoint Server Edition";
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
					sub = "Essential Server Solution Management Edition";
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
					sub = "Essential Server Solution Additional Edition";
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
					sub = "Essential Server Solution Management SVC Edition";
					break;
				case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
					sub = "Essential Server Solution Additional SVC Edition";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
					sub = "Small Business Server Premium Edition (core installation)";
					break;
				case PRODUCT_CLUSTER_SERVER_V:
					sub = "Hyper Core V Edition";
					break;
				case PRODUCT_STARTER_E:
					sub = "Hyper Core V Edition";
					break;
				case PRODUCT_ENTERPRISE_EVALUATION:
					sub = "Enterprise Edition (evaluation installation)";
					break;
				case PRODUCT_MULTIPOINT_STANDARD_SERVER:
					sub = "MultiPoint Server Standard Edition (full installation)";
					break;
				case PRODUCT_MULTIPOINT_PREMIUM_SERVER:
					sub = "MultiPoint Server Premium Edition (full installation)";
					break;
				case PRODUCT_STANDARD_EVALUATION_SERVER:
					sub = "Standard Edition (evaluation installation)";
					break;
				case PRODUCT_DATACENTER_EVALUATION_SERVER:
					sub = "Datacenter Edition (evaluation installation)";
					break;
				case PRODUCT_ENTERPRISE_N_EVALUATION:
					sub = "Enterprise N Edition (evaluation installation)";
					break;
				case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
					sub = "Storage Server Workgroup Edition (evaluation installation)";
					break;
				case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
					sub = "Storage Server Standard Edition (evaluation installation)";
					break;
				case PRODUCT_CORE_N:
					sub = "Windows 8 N Edition";
					break;
				case PRODUCT_CORE_COUNTRYSPECIFIC:
					sub = "Windows 8 China Edition";
					break;
				case PRODUCT_CORE_SINGLELANGUAGE:
					sub = "Windows 8 Single Language Edition";
					break;
				case PRODUCT_CORE:
					sub = "Windows 8 Edition";
					break;
				case PRODUCT_PROFESSIONAL_WMC:
					sub = "Professional with Media Center Edition";
					break;
			}
		}
	} else {
		return NULL;
	}

	spprintf(&retval, 0, "%s%s%s%s%s", major, sub?" ":"", sub?sub:"", osvi.szCSDVersion[0] != '\0'?" ":"", osvi.szCSDVersion);
	return retval;
}
/* }}}  */

/* {{{  */
void php_get_windows_cpu(char *buf, int bufsize)
{
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	switch (SysInfo.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_INTEL :
			snprintf(buf, bufsize, "i%d", SysInfo.dwProcessorType);
			break;
		case PROCESSOR_ARCHITECTURE_MIPS :
			snprintf(buf, bufsize, "MIPS R%d000", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA :
			snprintf(buf, bufsize, "Alpha %d", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_PPC :
			snprintf(buf, bufsize, "PPC 6%02d", SysInfo.wProcessorLevel);
			break;
		case PROCESSOR_ARCHITECTURE_IA64 :
			snprintf(buf, bufsize,  "IA64");
			break;
#if defined(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
		case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64 :
			snprintf(buf, bufsize, "IA32");
			break;
#endif
#if defined(PROCESSOR_ARCHITECTURE_AMD64)
		case PROCESSOR_ARCHITECTURE_AMD64 :
			snprintf(buf, bufsize, "AMD64");
			break;
#endif
		case PROCESSOR_ARCHITECTURE_UNKNOWN :
		default:
			snprintf(buf, bufsize, "Unknown");
			break;
	}
}
/* }}}  */
#endif

/* {{{ php_get_uname
 */
PHPAPI zend_string *php_get_uname(char mode)
{
	char *php_uname;
	char tmp_uname[256];
#ifdef PHP_WIN32
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	char ComputerName[MAX_COMPUTERNAME_LENGTH + 1];

	GetComputerName(ComputerName, &dwSize);

	if (mode == 's') {
		php_uname = "Windows NT";
	} else if (mode == 'r') {
		snprintf(tmp_uname, sizeof(tmp_uname), "%d.%d", dwWindowsMajorVersion, dwWindowsMinorVersion);
		php_uname = tmp_uname;
	} else if (mode == 'n') {
		php_uname = ComputerName;
	} else if (mode == 'v') {
		char *winver = php_get_windows_name();
		dwBuild = (DWORD)(HIWORD(dwVersion));
		if(winver == NULL) {
			snprintf(tmp_uname, sizeof(tmp_uname), "build %d", dwBuild);
		} else {
			snprintf(tmp_uname, sizeof(tmp_uname), "build %d (%s)", dwBuild, winver);
		}
		php_uname = tmp_uname;
		if(winver) {
			efree(winver);
		}
	} else if (mode == 'm') {
		php_get_windows_cpu(tmp_uname, sizeof(tmp_uname));
		php_uname = tmp_uname;
	} else { /* assume mode == 'a' */
		char *winver = php_get_windows_name();
		char wincpu[20];

		ZEND_ASSERT(winver != NULL);

		php_get_windows_cpu(wincpu, sizeof(wincpu));
		dwBuild = (DWORD)(HIWORD(dwVersion));

		/* Windows "version" 6.2 could be Windows 8/Windows Server 2012, but also Windows 8.1/Windows Server 2012 R2 */
		if (dwWindowsMajorVersion == 6 && dwWindowsMinorVersion == 2) {
			if (strncmp(winver, "Windows 8.1", 11) == 0 || strncmp(winver, "Windows Server 2012 R2", 22) == 0) {
				dwWindowsMinorVersion = 3;
			}
		}

		snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d build %d (%s) %s",
				 "Windows NT", ComputerName,
				 dwWindowsMajorVersion, dwWindowsMinorVersion, dwBuild, winver?winver:"unknown", wincpu);
		if(winver) {
			efree(winver);
		}
		php_uname = tmp_uname;
	}
#else
#ifdef HAVE_SYS_UTSNAME_H
	struct utsname buf;
	if (uname((struct utsname *)&buf) == -1) {
		php_uname = PHP_UNAME;
	} else {
#ifdef NETWARE
		if (mode == 's') {
			php_uname = buf.sysname;
		} else if (mode == 'r') {
			snprintf(tmp_uname, sizeof(tmp_uname), "%d.%d.%d",
					 buf.netware_major, buf.netware_minor, buf.netware_revision);
			php_uname = tmp_uname;
		} else if (mode == 'n') {
			php_uname = buf.servername;
		} else if (mode == 'v') {
			snprintf(tmp_uname, sizeof(tmp_uname), "libc-%d.%d.%d #%d",
					 buf.libmajor, buf.libminor, buf.librevision, buf.libthreshold);
			php_uname = tmp_uname;
		} else if (mode == 'm') {
			php_uname = buf.machine;
		} else { /* assume mode == 'a' */
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d.%d libc-%d.%d.%d #%d %s",
					 buf.sysname, buf.servername,
					 buf.netware_major, buf.netware_minor, buf.netware_revision,
					 buf.libmajor, buf.libminor, buf.librevision, buf.libthreshold,
					 buf.machine);
			php_uname = tmp_uname;
		}
#else
		if (mode == 's') {
			php_uname = buf.sysname;
		} else if (mode == 'r') {
			php_uname = buf.release;
		} else if (mode == 'n') {
			php_uname = buf.nodename;
		} else if (mode == 'v') {
			php_uname = buf.version;
		} else if (mode == 'm') {
			php_uname = buf.machine;
		} else { /* assume mode == 'a' */
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %s %s %s",
					 buf.sysname, buf.nodename, buf.release, buf.version,
					 buf.machine);
			php_uname = tmp_uname;
		}
#endif /* NETWARE */
	}
#else
	php_uname = PHP_UNAME;
#endif
#endif
	return zend_string_init(php_uname, strlen(php_uname), 0);
}
/* }}} */

/* {{{ php_print_info_htmlhead
 */
PHPAPI void php_print_info_htmlhead(void)
{
	php_info_print("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">\n");
	php_info_print("<html xmlns=\"http://www.w3.org/1999/xhtml\">");
	php_info_print("<head>\n");
	php_info_print_style();
	php_info_print("<title>phpinfo()</title>");
	php_info_print("<meta name=\"ROBOTS\" content=\"NOINDEX,NOFOLLOW,NOARCHIVE\" />");
	php_info_print("</head>\n");
	php_info_print("<body><div class=\"center\">\n");
}
/* }}} */

/* {{{ module_name_cmp */
static int module_name_cmp(const void *a, const void *b)
{
	Bucket *f = (Bucket *) a;
	Bucket *s = (Bucket *) b;

	return strcasecmp(((zend_module_entry *)Z_PTR(f->val))->name,
				  ((zend_module_entry *)Z_PTR(s->val))->name);
}
/* }}} */

/* {{{ php_print_info
 */
PHPAPI void php_print_info(int flag)
{
	char **env, *tmp1, *tmp2;
	zend_string *php_uname;

	if (!sapi_module.phpinfo_as_text) {
		php_print_info_htmlhead();
	} else {
		php_info_print("phpinfo()\n");
	}

	if (flag & PHP_INFO_GENERAL) {
		char *zend_version = get_zend_version();
		char temp_api[10];

		php_uname = php_get_uname('a');

		if (!sapi_module.phpinfo_as_text) {
			php_info_print_box_start(1);
		}

		if (!sapi_module.phpinfo_as_text) {
	        time_t the_time;
	        struct tm *ta, tmbuf;

	        the_time = time(NULL);
	        ta = php_localtime_r(&the_time, &tmbuf);

            php_info_print("<a href=\"http://www.php.net/\"><img border=\"0\" src=\"");
	        if (ta && (ta->tm_mon==3) && (ta->tm_mday==1)) {
		        php_info_print(PHP_EGG_LOGO_DATA_URI "\" alt=\"PHP logo\" /></a>");
	        } else {
		        php_info_print(PHP_LOGO_DATA_URI "\" alt=\"PHP logo\" /></a>");
			}
		}

		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<h1 class=\"p\">PHP Version %s</h1>\n", PHP_VERSION);
		} else {
			php_info_print_table_row(2, "PHP Version", PHP_VERSION);
		}
		php_info_print_box_end();
		php_info_print_table_start();
		php_info_print_table_row(2, "System", ZSTR_VAL(php_uname));
		php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__);
#ifdef COMPILER
		php_info_print_table_row(2, "Compiler", COMPILER);
#endif
#ifdef ARCHITECTURE
		php_info_print_table_row(2, "Architecture", ARCHITECTURE);
#endif
#ifdef CONFIGURE_COMMAND
		php_info_print_table_row(2, "Configure Command", CONFIGURE_COMMAND );
#endif

		if (sapi_module.pretty_name) {
			php_info_print_table_row(2, "Server API", sapi_module.pretty_name );
		}

#ifdef VIRTUAL_DIR
		php_info_print_table_row(2, "Virtual Directory Support", "enabled" );
#else
		php_info_print_table_row(2, "Virtual Directory Support", "disabled" );
#endif

		php_info_print_table_row(2, "Configuration File (php.ini) Path", PHP_CONFIG_FILE_PATH);
		php_info_print_table_row(2, "Loaded Configuration File", php_ini_opened_path ? php_ini_opened_path : "(none)");
		php_info_print_table_row(2, "Scan this dir for additional .ini files", php_ini_scanned_path ? php_ini_scanned_path : "(none)");
		php_info_print_table_row(2, "Additional .ini files parsed", php_ini_scanned_files ? php_ini_scanned_files : "(none)");

		snprintf(temp_api, sizeof(temp_api), "%d", PHP_API_VERSION);
		php_info_print_table_row(2, "PHP API", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_MODULE_API_NO);
		php_info_print_table_row(2, "PHP Extension", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_EXTENSION_API_NO);
		php_info_print_table_row(2, "Zend Extension", temp_api);

		php_info_print_table_row(2, "Zend Extension Build", ZEND_EXTENSION_BUILD_ID);
		php_info_print_table_row(2, "PHP Extension Build", ZEND_MODULE_BUILD_ID);

#if ZEND_DEBUG
		php_info_print_table_row(2, "Debug Build", "yes" );
#else
		php_info_print_table_row(2, "Debug Build", "no" );
#endif

#ifdef ZTS
		php_info_print_table_row(2, "Thread Safety", "enabled" );
#else
		php_info_print_table_row(2, "Thread Safety", "disabled" );
#endif

#ifdef ZEND_SIGNALS
		php_info_print_table_row(2, "Zend Signal Handling", "enabled" );
#else
		php_info_print_table_row(2, "Zend Signal Handling", "disabled" );
#endif

		php_info_print_table_row(2, "Zend Memory Manager", is_zend_mm() ? "enabled" : "disabled" );

		{
			const zend_multibyte_functions *functions = zend_multibyte_get_functions();
			char *descr;
			if (functions) {
				spprintf(&descr, 0, "provided by %s", functions->provider_name);
			} else {
				descr = estrdup("disabled");
			}
            php_info_print_table_row(2, "Zend Multibyte Support", descr);
			efree(descr);
		}

#if HAVE_IPV6
		php_info_print_table_row(2, "IPv6 Support", "enabled" );
#else
		php_info_print_table_row(2, "IPv6 Support", "disabled" );
#endif

#if HAVE_DTRACE
		php_info_print_table_row(2, "DTrace Support", (zend_dtrace_enabled ? "enabled" : "available, disabled"));
#else
		php_info_print_table_row(2, "DTrace Support", "disabled" );
#endif

		php_info_print_stream_hash("PHP Streams",  php_stream_get_url_stream_wrappers_hash());
		php_info_print_stream_hash("Stream Socket Transports", php_stream_xport_get_hash());
		php_info_print_stream_hash("Stream Filters", php_get_stream_filters_hash());

		php_info_print_table_end();

		/* Zend Engine */
		php_info_print_box_start(0);
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<a href=\"http://www.zend.com/\"><img border=\"0\" src=\"");
			php_info_print(ZEND_LOGO_DATA_URI "\" alt=\"Zend logo\" /></a>\n");
		}
		php_info_print("This program makes use of the Zend Scripting Language Engine:");
		php_info_print(!sapi_module.phpinfo_as_text?"<br />":"\n");
		if (sapi_module.phpinfo_as_text) {
			php_info_print(zend_version);
		} else {
			zend_html_puts(zend_version, strlen(zend_version));
		}
		php_info_print_box_end();
		zend_string_free(php_uname);
	}

	zend_ini_sort_entries();

	if (flag & PHP_INFO_CONFIGURATION) {
		php_info_print_hr();
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<h1>Configuration</h1>\n");
		} else {
			SECTION("Configuration");
		}
		if (!(flag & PHP_INFO_MODULES)) {
			SECTION("PHP Core");
			display_ini_entries(NULL);
		}
	}

	if (flag & PHP_INFO_MODULES) {
		HashTable sorted_registry;

		zend_hash_init(&sorted_registry, zend_hash_num_elements(&module_registry), NULL, NULL, 1);
		zend_hash_copy(&sorted_registry, &module_registry, NULL);
		zend_hash_sort(&sorted_registry, module_name_cmp, 0);

		zend_hash_apply(&sorted_registry, _display_module_info_func);

		SECTION("Additional Modules");
		php_info_print_table_start();
		php_info_print_table_header(1, "Module Name");
		zend_hash_apply(&sorted_registry, _display_module_info_def);
		php_info_print_table_end();

		zend_hash_destroy(&sorted_registry);
	}

	if (flag & PHP_INFO_ENVIRONMENT) {
		SECTION("Environment");
		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		for (env=environ; env!=NULL && *env !=NULL; env++) {
			tmp1 = estrdup(*env);
			if (!(tmp2=strchr(tmp1,'='))) { /* malformed entry? */
				efree(tmp1);
				continue;
			}
			*tmp2 = 0;
			tmp2++;
			php_info_print_table_row(2, tmp1, tmp2);
			efree(tmp1);
		}
		php_info_print_table_end();
	}

	if (flag & PHP_INFO_VARIABLES) {
		zval *data;

		SECTION("PHP Variables");

		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		if ((data = zend_hash_str_find(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF")-1)) != NULL && Z_TYPE_P(data) == IS_STRING) {
			php_info_print_table_row(2, "PHP_SELF", Z_STRVAL_P(data));
		}
		if ((data = zend_hash_str_find(&EG(symbol_table), "PHP_AUTH_TYPE", sizeof("PHP_AUTH_TYPE")-1)) != NULL && Z_TYPE_P(data) == IS_STRING) {
			php_info_print_table_row(2, "PHP_AUTH_TYPE", Z_STRVAL_P(data));
		}
		if ((data = zend_hash_str_find(&EG(symbol_table), "PHP_AUTH_USER", sizeof("PHP_AUTH_USER")-1)) != NULL && Z_TYPE_P(data) == IS_STRING) {
			php_info_print_table_row(2, "PHP_AUTH_USER", Z_STRVAL_P(data));
		}
		if ((data = zend_hash_str_find(&EG(symbol_table), "PHP_AUTH_PW", sizeof("PHP_AUTH_PW")-1)) != NULL && Z_TYPE_P(data) == IS_STRING) {
			php_info_print_table_row(2, "PHP_AUTH_PW", Z_STRVAL_P(data));
		}
		php_print_gpcse_array(ZEND_STRL("_REQUEST"));
		php_print_gpcse_array(ZEND_STRL("_GET"));
		php_print_gpcse_array(ZEND_STRL("_POST"));
		php_print_gpcse_array(ZEND_STRL("_FILES"));
		php_print_gpcse_array(ZEND_STRL("_COOKIE"));
		php_print_gpcse_array(ZEND_STRL("_SERVER"));
		php_print_gpcse_array(ZEND_STRL("_ENV"));
		php_info_print_table_end();
	}


	if ((flag & PHP_INFO_CREDITS) && !sapi_module.phpinfo_as_text) {
		php_info_print_hr();
		php_print_credits(PHP_CREDITS_ALL & ~PHP_CREDITS_FULLPAGE);
	}

	if (flag & PHP_INFO_LICENSE) {
		if (!sapi_module.phpinfo_as_text) {
			SECTION("PHP License");
			php_info_print_box_start(0);
			php_info_print("<p>\n");
			php_info_print("This program is free software; you can redistribute it and/or modify ");
			php_info_print("it under the terms of the PHP License as published by the PHP Group ");
			php_info_print("and included in the distribution in the file:  LICENSE\n");
			php_info_print("</p>\n");
			php_info_print("<p>");
			php_info_print("This program is distributed in the hope that it will be useful, ");
			php_info_print("but WITHOUT ANY WARRANTY; without even the implied warranty of ");
			php_info_print("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			php_info_print("</p>\n");
			php_info_print("<p>");
			php_info_print("If you did not receive a copy of the PHP license, or have any questions about ");
			php_info_print("PHP licensing, please contact license@php.net.\n");
			php_info_print("</p>\n");
			php_info_print_box_end();
		} else {
			php_info_print("\nPHP License\n");
			php_info_print("This program is free software; you can redistribute it and/or modify\n");
			php_info_print("it under the terms of the PHP License as published by the PHP Group\n");
			php_info_print("and included in the distribution in the file:  LICENSE\n");
			php_info_print("\n");
			php_info_print("This program is distributed in the hope that it will be useful,\n");
			php_info_print("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
			php_info_print("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			php_info_print("\n");
			php_info_print("If you did not receive a copy of the PHP license, or have any\n");
			php_info_print("questions about PHP licensing, please contact license@php.net.\n");
		}
	}

	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</div></body></html>");
	}
}
/* }}} */

PHPAPI void php_info_print_table_start(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<table>\n");
	} else {
		php_info_print("\n");
	}
}
/* }}} */

PHPAPI void php_info_print_table_end(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</table>\n");
	}

}
/* }}} */

PHPAPI void php_info_print_box_start(int flag) /* {{{ */
{
	php_info_print_table_start();
	if (flag) {
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<tr class=\"h\"><td>\n");
		}
	} else {
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<tr class=\"v\"><td>\n");
		} else {
			php_info_print("\n");
		}
	}
}
/* }}} */

PHPAPI void php_info_print_box_end(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</td></tr>\n");
	}
	php_info_print_table_end();
}
/* }}} */

PHPAPI void php_info_print_hr(void) /* {{{ */
{
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<hr />\n");
	} else {
		php_info_print("\n\n _______________________________________________________________________\n\n");
	}
}
/* }}} */

PHPAPI void php_info_print_table_colspan_header(int num_cols, char *header) /* {{{ */
{
	int spaces;

	if (!sapi_module.phpinfo_as_text) {
		php_info_printf("<tr class=\"h\"><th colspan=\"%d\">%s</th></tr>\n", num_cols, header );
	} else {
		spaces = (int)(74 - strlen(header));
		php_info_printf("%*s%s%*s\n", (int)(spaces/2), " ", header, (int)(spaces/2), " ");
	}
}
/* }}} */

/* {{{ php_info_print_table_header
 */
PHPAPI void php_info_print_table_header(int num_cols, ...)
{
	int i;
	va_list row_elements;
	char *row_element;

	va_start(row_elements, num_cols);
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<tr class=\"h\">");
	}
	for (i=0; i<num_cols; i++) {
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			row_element = " ";
		}
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<th>");
			php_info_print(row_element);
			php_info_print("</th>");
		} else {
			php_info_print(row_element);
			if (i < num_cols-1) {
				php_info_print(" => ");
			} else {
				php_info_print("\n");
			}
		}
	}
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</tr>\n");
	}

	va_end(row_elements);
}
/* }}} */

/* {{{ php_info_print_table_row_internal
 */
static void php_info_print_table_row_internal(int num_cols,
		const char *value_class, va_list row_elements)
{
	int i;
	char *row_element;

	if (!sapi_module.phpinfo_as_text) {
		php_info_print("<tr>");
	}
	for (i=0; i<num_cols; i++) {
		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<td class=\"%s\">",
			   (i==0 ? "e" : value_class )
			);
		}
		row_element = va_arg(row_elements, char *);
		if (!row_element || !*row_element) {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print( "<i>no value</i>" );
			} else {
				php_info_print( " " );
			}
		} else {
			if (!sapi_module.phpinfo_as_text) {
				php_info_print_html_esc(row_element, strlen(row_element));
			} else {
				php_info_print(row_element);
				if (i < num_cols-1) {
					php_info_print(" => ");
				}
			}
		}
		if (!sapi_module.phpinfo_as_text) {
			php_info_print(" </td>");
		} else if (i == (num_cols - 1)) {
			php_info_print("\n");
		}
	}
	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</tr>\n");
	}
}
/* }}} */

/* {{{ php_info_print_table_row
 */
PHPAPI void php_info_print_table_row(int num_cols, ...)
{
	va_list row_elements;

	va_start(row_elements, num_cols);
	php_info_print_table_row_internal(num_cols, "v", row_elements);
	va_end(row_elements);
}
/* }}} */

/* {{{ php_info_print_table_row_ex
 */
PHPAPI void php_info_print_table_row_ex(int num_cols, const char *value_class,
		...)
{
	va_list row_elements;

	va_start(row_elements, value_class);
	php_info_print_table_row_internal(num_cols, value_class, row_elements);
	va_end(row_elements);
}
/* }}} */

/* {{{ register_phpinfo_constants
 */
void register_phpinfo_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("INFO_GENERAL", PHP_INFO_GENERAL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CREDITS", PHP_INFO_CREDITS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_CONFIGURATION", PHP_INFO_CONFIGURATION, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_MODULES", PHP_INFO_MODULES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ENVIRONMENT", PHP_INFO_ENVIRONMENT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_VARIABLES", PHP_INFO_VARIABLES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_LICENSE", PHP_INFO_LICENSE, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("INFO_ALL", PHP_INFO_ALL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_GROUP",	PHP_CREDITS_GROUP, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_GENERAL",	PHP_CREDITS_GENERAL, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_SAPI",	PHP_CREDITS_SAPI, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_MODULES",	PHP_CREDITS_MODULES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_DOCS",	PHP_CREDITS_DOCS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_FULLPAGE",	PHP_CREDITS_FULLPAGE, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_QA",	PHP_CREDITS_QA, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("CREDITS_ALL",	PHP_CREDITS_ALL, CONST_PERSISTENT|CONST_CS);
}
/* }}} */

/* {{{ proto void phpinfo([int what])
   Output a page of useful information about PHP and the current request */
PHP_FUNCTION(phpinfo)
{
	zend_long flag = PHP_INFO_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flag) == FAILURE) {
		return;
	}

	/* Andale!  Andale!  Yee-Hah! */
	php_output_start_default();
	php_print_info((int)flag);
	php_output_end();

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto string phpversion([string extension])
   Return the current PHP version */
PHP_FUNCTION(phpversion)
{
	char *ext_name = NULL;
	size_t ext_name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &ext_name, &ext_name_len) == FAILURE) {
		return;
	}

	if (!ext_name) {
		RETURN_STRING(PHP_VERSION);
	} else {
		const char *version;
		version = zend_get_module_version(ext_name);
		if (version == NULL) {
			RETURN_FALSE;
		}
		RETURN_STRING(version);
	}
}
/* }}} */

/* {{{ proto void phpcredits([int flag])
   Prints the list of people who've contributed to the PHP project */
PHP_FUNCTION(phpcredits)
{
	zend_long flag = PHP_CREDITS_ALL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flag) == FAILURE) {
		return;
	}

	php_print_credits((int)flag);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string php_sapi_name(void)
   Return the current SAPI module name */
PHP_FUNCTION(php_sapi_name)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sapi_module.name) {
		RETURN_STRING(sapi_module.name);
	} else {
		RETURN_FALSE;
	}
}

/* }}} */

/* {{{ proto string php_uname(void)
   Return information about the system PHP was built on */
PHP_FUNCTION(php_uname)
{
	char *mode = "a";
	size_t modelen = sizeof("a")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &mode, &modelen) == FAILURE) {
		return;
	}
	RETURN_STR(php_get_uname(*mode));
}

/* }}} */

/* {{{ proto string php_ini_scanned_files(void)
   Return comma-separated string of .ini files parsed from the additional ini dir */
PHP_FUNCTION(php_ini_scanned_files)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (strlen(PHP_CONFIG_FILE_SCAN_DIR) && php_ini_scanned_files) {
		RETURN_STRING(php_ini_scanned_files);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string php_ini_loaded_file(void)
   Return the actual loaded ini filename */
PHP_FUNCTION(php_ini_loaded_file)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_ini_opened_path) {
		RETURN_STRING(php_ini_opened_path);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

// Utils for configure script
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

// $Id: confutils.js,v 1.60.2.1.2.8.2.33 2009-05-29 07:43:07 kalle Exp $

var STDOUT = WScript.StdOut;
var STDERR = WScript.StdErr;
var WshShell = WScript.CreateObject("WScript.Shell");
var FSO = WScript.CreateObject("Scripting.FileSystemObject");
var MFO = null;
var SYSTEM_DRIVE = WshShell.Environment("Process").Item("SystemDrive");
var PROGRAM_FILES = WshShell.Environment("Process").Item("ProgramFiles");
var DSP_FLAGS = new Array();
var PHP_SRC_DIR=FSO.GetParentFolderName(WScript.ScriptFullName);

/* Store the enabled extensions (summary + QA check) */
var extensions_enabled = new Array();

/* Store the SAPI enabled (summary + QA check) */
var sapi_enabled = new Array();

/* Store the headers to install */
var headers_install = new Array();

/* Mapping CL version > human readable name */
var VC_VERSIONS = new Array();
VC_VERSIONS[1200] = 'MSVC6 (Visual C++ 6.0)';
VC_VERSIONS[1300] = 'MSVC7 (Visual C++ 2002)';
VC_VERSIONS[1310] = 'MSVC7.1 (Visual C++ 2003)';
VC_VERSIONS[1400] = 'MSVC8 (Visual C++ 2005)';
VC_VERSIONS[1500] = 'MSVC9 (Visual C++ 2008)';
VC_VERSIONS[1600] = 'MSVC10 (Visual C++ 2010)';

var VC_VERSIONS_SHORT = new Array();
VC_VERSIONS_SHORT[1200] = 'VC6';
VC_VERSIONS_SHORT[1300] = 'VC7';
VC_VERSIONS_SHORT[1310] = 'VC7.1';
VC_VERSIONS_SHORT[1400] = 'VC8';
VC_VERSIONS_SHORT[1500] = 'VC9';
VC_VERSIONS_SHORT[1600] = 'VC10';

if (PROGRAM_FILES == null) {
	PROGRAM_FILES = "C:\\Program Files";
}

if (MODE_PHPIZE) {
	if (!FSO.FileExists("config.w32")) {
		STDERR.WriteLine("Must be run from the root of the extension source");
		WScript.Quit(10);
	}
} else {
	if (!FSO.FileExists("README.SVN-RULES")) {
		STDERR.WriteLine("Must be run from the root of the php source");
		WScript.Quit(10);
	}
}

var CWD = WshShell.CurrentDirectory;

if (typeof(CWD) == "undefined") {
	CWD = FSO.GetParentFolderName(FSO.GetAbsolutePathName("README.SVN-RULES"));
}

/* defaults; we pick up the precise versions from configure.in */
var PHP_VERSION = 5;
var PHP_MINOR_VERSION = 0;
var PHP_RELEASE_VERSION = 0;
var PHP_EXTRA_VERSION = "";
var PHP_VERSION_STRING = "5.0.0";

function get_version_numbers()
{
	var cin = file_get_contents("configure.in");
	
	if (cin.match(new RegExp("PHP_MAJOR_VERSION=(\\d+)"))) {
		PHP_VERSION = RegExp.$1;
	}
	if (cin.match(new RegExp("PHP_MINOR_VERSION=(\\d+)"))) {
		PHP_MINOR_VERSION = RegExp.$1;
	}
	if (cin.match(new RegExp("PHP_RELEASE_VERSION=(\\d+)"))) {
		PHP_RELEASE_VERSION = RegExp.$1;
	}
	PHP_VERSION_STRING = PHP_VERSION + "." + PHP_MINOR_VERSION + "." + PHP_RELEASE_VERSION;

	if (cin.match(new RegExp("PHP_EXTRA_VERSION=\"([^\"]+)\""))) {
		PHP_EXTRA_VERSION = RegExp.$1;
		if (PHP_EXTRA_VERSION.length) {
			PHP_VERSION_STRING += PHP_EXTRA_VERSION;
		}
	}
	DEFINE('PHP_VERSION_STRING', PHP_VERSION_STRING);
}

configure_args = new Array();
configure_subst = WScript.CreateObject("Scripting.Dictionary");

configure_hdr = WScript.CreateObject("Scripting.Dictionary");
build_dirs = new Array();

extension_include_code = "";
extension_module_ptrs = "";

if (!MODE_PHPIZE) {
	get_version_numbers();
}

/* execute a command and return the output as a string */
function execute(command_line)
{
	var e = WshShell.Exec(command_line);
	var ret = "";

	ret = e.StdOut.ReadAll();

//STDOUT.WriteLine("command " + command_line);
//STDOUT.WriteLine(ret);

	return ret;
}

function probe_binary(EXE, what)
{
	// tricky escapes to get stderr redirection to work
	var command = 'cmd /c ""' + EXE;
	if (what == "version") {
		command = command + '" -v"';
	}
	var version = execute(command + '" 2>&1"');

	if (what == "64") {
		if (version.match(/x64/)) {
			return 1;
		}
	} else {
		if (version.match(/(\d+\.\d+(\.\d+)?(\.\d+)?)/)) {
			return RegExp.$1;
		}
	}
	return 0;
}

function condense_path(path)
{
	path = FSO.GetAbsolutePathName(path);

	if (path.substr(0, CWD.length).toLowerCase()
			== CWD.toLowerCase() &&
			(path.charCodeAt(CWD.length) == 92 || path.charCodeAt(CWD.length) == 47)) {
		return path.substr(CWD.length + 1);
	}

	var a = CWD.split("\\");
	var b = path.split("\\");
	var i, j;

	for (i = 0; i < b.length; i++) {
		if (a[i].toLowerCase() == b[i].toLowerCase())
			continue;
		if (i > 0) {
			/* first difference found */
			path = "";
			for (j = 0; j < a.length - i; j++) {
				path += "..\\";
			}
			for (j = i; j < b.length; j++) {
				path += b[j];
				if (j < b.length - 1)
					path += "\\";
			}
			return path;
		}
		/* on a different drive */
		break;
	}
	
	return path;
}

function ConfigureArg(type, optname, helptext, defval)
{
	var opptype = type == "enable" ? "disable" : "without";

	if (defval == "yes" || defval == "yes,shared") {
		this.arg = "--" + opptype + "-" + optname;
		this.imparg = "--" + type + "-" + optname;
	} else {
		this.arg = "--" + type + "-" + optname;
		this.imparg = "--" + opptype + "-" + optname;
	}
	
	this.optname = optname;
	this.helptext = helptext;
	this.defval = defval;
	this.symval = optname.toUpperCase().replace(new RegExp("-", "g"), "_");
	this.seen = false;
	this.argval = defval;
}

function ARG_WITH(optname, helptext, defval)
{
	configure_args[configure_args.length] = new ConfigureArg("with", optname, helptext, defval);
}

function ARG_ENABLE(optname, helptext, defval)
{
	configure_args[configure_args.length] = new ConfigureArg("enable", optname, helptext, defval);
}

function analyze_arg(argval)
{
	var ret = new Array();
	var shared = false;

	if (argval == "shared") {
		shared = true;
		argval = "yes";
	} else if (argval == null) {
		/* nothing */
	} else if (arg_match = argval.match(new RegExp("^shared,(.*)"))) {
		shared = true;
		argval = arg_match[1];
	} else if (arg_match = argval.match(new RegExp("^(.*),shared$"))) {
		shared = true;
		argval = arg_match[1];
	}

	ret[0] = shared;
	ret[1] = argval;
	return ret;
}

function word_wrap_and_indent(indent, text, line_suffix, indent_char)
{
	if (text == null) {
		return "";
	}
	
	var words = text.split(new RegExp("\\s+", "g"));
	var i = 0;
	var ret_text = "";
	var this_line = "";
	var t;
	var space = "";
	var lines = 0;

	if (line_suffix == null) {
		line_suffix = "";
	}

	if (indent_char == null) {
		indent_char = " ";
	}

	for (i = 0; i < indent; i++) {
		space += indent_char;
	}
	
	for (i = 0; i < words.length; i++) {
		if (this_line.length) {
			t = this_line + " " + words[i];
		} else {
			t = words[i];
		}

		if (t.length + indent > 78) {
			if (lines++) {
				ret_text += space;
			}
			ret_text += this_line + line_suffix + "\r\n";
			this_line = "";
		}

		if (this_line.length) {
			this_line += " " + words[i];
		} else {
			this_line = words[i];
		}
	}

	if (this_line.length) {
		if (lines)
			ret_text += space;
		ret_text += this_line;
	}

	return ret_text;
}

function conf_process_args()
{
	var i, j;
	var configure_help_mode = false;
	var analyzed = false;
	var nice = "cscript /nologo configure.js ";
	var disable_all = false;
	
	args = WScript.Arguments;
	for (i = 0; i < args.length; i++) {
		arg = args(i);
		nice += ' "' + arg + '"';
		if (arg == "--help") {
			configure_help_mode = true;
			break;
		}
		if (arg == "--disable-all") {
			disable_all = true;
			continue;
		}

		// If it is --foo=bar, split on the equals sign
		arg = arg.split("=", 2);
		argname = arg[0];
		if (arg.length > 1) {
			argval = arg[1];
		} else {
			argval = null;
		}

		// Find the arg
		found = false;
		for (j = 0; j < configure_args.length; j++) {
			if (argname == configure_args[j].imparg || argname == configure_args[j].arg) {
				found = true;

				arg = configure_args[j];
				arg.seen = true;

				analyzed = analyze_arg(argval);

				/* Force shared when called after phpize */
				if (MODE_PHPIZE) {
					shared = "shared";
				} else {
					shared = analyzed[0];
				}
				argval = analyzed[1];

				if (argname == arg.imparg) {
					/* we matched the implicit, or default arg */
					if (argval == null) {
						argval = arg.defval;
					}
				} else {
					/* we matched the non-default arg */
					if (argval == null) {
						argval = arg.defval == "no" ? "yes" : "no";
					}
				}
				
				arg.argval = argval;
				eval("PHP_" + arg.symval + " = argval;");
				eval("PHP_" + arg.symval + "_SHARED = shared;");
				break;
			}
		}
		if (!found) {
			STDERR.WriteLine("Unknown option " + argname + "; please try configure.js --help for a list of valid options");
			WScript.Quit(2);
		}
	}

	if (configure_help_mode) {
		STDOUT.WriteLine(word_wrap_and_indent(0,
"Options that enable extensions and SAPI will accept \
'yes' or 'no' as a parameter.  They also accept 'shared' \
as a synonym for 'yes' and request a shared build of that \
module.  Not all modules can be built as shared modules; \
configure will display [shared] after the module name if \
can be built that way. \
"
			));
		STDOUT.WriteBlankLines(1);

		// Measure width to pretty-print the output
		max_width = 0;
		for (i = 0; i < configure_args.length; i++) {
			arg = configure_args[i];
			if (arg.arg.length > max_width)
				max_width = arg.arg.length;
		}

		for (i = 0; i < configure_args.length; i++) {
			arg = configure_args[i];

			n = max_width - arg.arg.length;
			pad = "   ";
			for (j = 0; j < n; j++) {
				pad += " ";
			}
			STDOUT.WriteLine("  " + arg.arg + pad + word_wrap_and_indent(max_width + 5, arg.helptext));
		}
		WScript.Quit(1);
	}

	var snapshot_build_exclusions = new Array(
		'debug', 'crt-debug', 'lzf-better-compression',
		 'php-build', 'snapshot-template', 'ereg',
		 'pcre-regex', 'fastcgi', 'force-cgi-redirect',
		 'path-info-check', 'zts', 'ipv6', 'memory-limit',
		 'zend-multibyte', 'fd-setsize', 'memory-manager', 't1lib'
		);
	var force;

	// Now set any defaults we might have missed out earlier
	for (i = 0; i < configure_args.length; i++) {
		arg = configure_args[i];
		if (arg.seen)
			continue;
		analyzed = analyze_arg(arg.defval);
		shared = analyzed[0];
		argval = analyzed[1];
		
		// Don't trust a default "yes" answer for a non-core module
		// in a snapshot build
		if (PHP_SNAPSHOT_BUILD != "no" && argval == "yes" && !shared) {

			force = true;
			for (j = 0; j < snapshot_build_exclusions.length; j++) {
				if (snapshot_build_exclusions[j] == arg.optname) {
					force = false;
					break;
				}
			}

			if (force) {
				/* now check if it is a core module */
				force = false;
				for (j = 0; j < core_module_list.length; j++) {
					if (core_module_list[j] == arg.optname) {
						force = true;
						break;
					}
				}

				if (!force) {
					STDOUT.WriteLine("snapshot: forcing " + arg.arg + " shared");
					shared = true;
				}
			}
		}
		
		if (PHP_SNAPSHOT_BUILD != "no" && argval == "no") {
			force = true;
			for (j = 0; j < snapshot_build_exclusions.length; j++) {
				if (snapshot_build_exclusions[j] == arg.optname) {
					force = false;
					break;
				}
			}
			if (force) {
				STDOUT.WriteLine("snapshot: forcing " + arg.optname + " on");
				argval = "yes";
				shared = true;
			}
		}

		if (disable_all) {
			force = true;
			for (j = 0; j < snapshot_build_exclusions.length; j++) {
				if (snapshot_build_exclusions[j] == arg.optname) {
					force = false;
					break;
				}
			}
			if (force) {
				if (arg.defval == '') {
					argval = '';
				} else {
					argval = "no";
				}
				shared = false;
			}
		}

		eval("PHP_" + arg.symval + " = argval;");
		eval("PHP_" + arg.symval + "_SHARED = shared;");
	}

	MFO = FSO.CreateTextFile("Makefile.objects", true);

	STDOUT.WriteLine("Saving configure options to config.nice.bat");
	var nicefile = FSO.CreateTextFile("config.nice.bat", true);
	nicefile.WriteLine(nice +  " %*");
	nicefile.Close();

	AC_DEFINE('CONFIGURE_COMMAND', nice, "Configure line");
}

function DEFINE(name, value)
{
	if (configure_subst.Exists(name)) {
		configure_subst.Remove(name);
	}
	configure_subst.Add(name, value);
}

// Searches a set of paths for a file;
// returns the dir in which the file was found,
// true if it was found in the default env path,
// or false if it was not found at all.
// env_name is the optional name of an env var
// specifying the default path to search
function search_paths(thing_to_find, explicit_path, env_name)
{
	var i, found = false, place = false, file, env;

	STDOUT.Write("Checking for " + thing_to_find + " ... ");

	thing_to_find = thing_to_find.replace(new RegExp("/", "g"), "\\");

	if (explicit_path != null) {
		if (typeof(explicit_path) == "string") {
			explicit_path = explicit_path.split(";");
		}

		for (i = 0; i < explicit_path.length; i++) {
			file = glob(explicit_path[i] + "\\" + thing_to_find);
			if (file) {
				found = true;
				place = file[0];
				place = place.substr(0, place.length - thing_to_find.length - 1);
				break;
			}
		}
	}

	if (!found && env_name != null) {
		env = WshShell.Environment("Process").Item(env_name);
		env = env.split(";");
		for (i = 0; i < env.length; i++) {
			file = glob(env[i] + "\\" + thing_to_find);
			if (file) {
				found = true;
				place = true;
				break;
			}
		}
	}

	if (found && place == true) {
		STDOUT.WriteLine(" <in default path>");
	} else if (found) {
		STDOUT.WriteLine(" " + place);
	} else {
		STDOUT.WriteLine(" <not found>");
	}
	return place;
}

function PATH_PROG(progname, additional_paths, symbol)
{
	var exe;
	var place;
	var cyg_path = PHP_CYGWIN + "\\bin;" + PHP_CYGWIN + "\\usr\\local\\bin";
	var php_build_bin_path = PHP_PHP_BUILD + "\\bin"

	exe = progname + ".exe";

	if (additional_paths == null) {
		additional_paths = cyg_path;
	} else {
		additional_paths += ";" + cyg_path;
	}

	additional_paths = additional_paths + ";" + php_build_bin_path;

	place = search_paths(exe, additional_paths, "PATH");

	if (place == true) {
		place = exe;
	} else if (place != false) {
		place = place + "\\" + exe;
	}

	if (place) {
		if (symbol == null) {
			symbol = progname.toUpperCase();
		}
		DEFINE(symbol, place);
	}
	return place;
}

function find_pattern_in_path(pattern, path)
{
	if (path == null) {
		return false;
	}

	var dirs = path.split(';');
	var i;
	var items;

	for (i = 0; i < dirs.length; i++) {
		items = glob(dirs[i] + "\\" + pattern);
		if (items) {
			return condense_path(items[0]);
		}
	}
	return false;
}

function CHECK_LIB(libnames, target, path_to_check, common_name)
{
	STDOUT.Write("Checking for library " + libnames + " ... ");

	if (common_name == null && target != null) {
		common_name = target;
	}

	if (path_to_check == null) {
		path_to_check = "";
	}

	// if they specified a common name for the package that contains
	// the library, tag some useful defaults on to the end of the
	// path to be searched
	if (common_name != null) {
		path_to_check += ";" + PHP_PHP_BUILD + "\\" + common_name + "*";
		path_to_check += ";" + PHP_PHP_BUILD + "\\lib\\" + common_name + "*";
		path_to_check += ";..\\" + common_name + "*";
	}

	// Determine target for build flags
	if (target == null) {
		target = "";
	} else {
		target = "_" + target.toUpperCase();
	}

	// Expand path to include general dirs
	path_to_check += ";" + php_usual_lib_suspects;

	// It is common practice to put libs under one of these dir names
	var subdirs = new Array(PHP_DEBUG == "yes" ? "Debug" : (PHP_DEBUG_PACK == "yes"?"Release_Dbg":"Release"), "lib", "libs", "libexec");

	// libnames can be ; separated list of accepted library names
	libnames = libnames.split(';');

	// for debug builds, lib may have _debug appended, we want that first
	if (PHP_DEBUG == "yes") {
		var length = libnames.length;
		for (var i = 0; i < length; i++) {
			var name = new String(libnames[i]);
			rExp = /.lib$/i;
			name = name.replace(rExp,"_debug.lib");
			libnames.unshift(name);
		}
	}

	var i, j, k, libname;
	var location = false;
	var path = path_to_check.split(';');
	
	for (i = 0; i < libnames.length; i++) {
		libname = libnames[i];

		for (k = 0; k < path.length; k++) {
			location = glob(path[k] + "\\" + libname);
			if (location) {
				location = location[0];
				break;
			}
			for (j = 0; j < subdirs.length; j++) {
				location = glob(path[k] + "\\" + subdirs[j] + "\\" + libname);
				if (location) {
					location = location[0];
					break;
				}
			}
			if (location)
				break;
		}

		if (location) {
			location = condense_path(location);
			var libdir = FSO.GetParentFolderName(location);
			libname = FSO.GetFileName(location);
			ADD_FLAG("LDFLAGS" + target, '/libpath:"' + libdir + '" ');
			ADD_FLAG("LIBS" + target, libname);

			STDOUT.WriteLine(location);

			return location;
		}

		// Check in their standard lib path
		location = find_pattern_in_path(libname, WshShell.Environment("Process").Item("LIB"));

		if (location) {
			location = condense_path(location);
			libname = FSO.GetFileName(location);
			ADD_FLAG("LIBS" + target, libname);

			STDOUT.WriteLine("<in LIB path> " + libname);
			return location;
		}

		// Check in their general extra libs path
		location = find_pattern_in_path(libname, PHP_EXTRA_LIBS);
		if (location) {
			location = condense_path(location);
			libname = FSO.GetFileName(location);
			ADD_FLAG("LIBS" + target, libname);
			STDOUT.WriteLine("<in extra libs path>");
			return location;
		}
	}

	STDOUT.WriteLine("<not found>");

	return false;
}

function OLD_CHECK_LIB(libnames, target, path_to_check)
{
	if (target == null) {
		target = "";
	} else {
		target = "_" + target.toUpperCase();
	}
	
	if (path_to_check == null) {
		path_to_check = php_usual_lib_suspects;
	} else {
		path_to_check += ";" + php_usual_lib_suspects;
	}
	var have = 0;
	var p;
	var i;
	var libname;

	var subdir = PHP_DEBUG == "yes" ? "Debug" : (PHP_DEBUG_PACK == "yes"?"Release_Dbg":"Release");

	libnames = libnames.split(';');
	for (i = 0; i < libnames.length; i++) {
		libname = libnames[i];
		p = search_paths(libname, path_to_check, "LIB");

		if (!p) {
			p = search_paths(subdir + "\\" + libname, path_to_check, "LIB");
			if (p) {
				p += "\\" + subdir;
			}
		}

		if (typeof(p) == "string") {
			ADD_FLAG("LDFLAGS" + target, '/libpath:"' + p + '" ');
			ADD_FLAG("LIBS" + target, libname);
			have = 1;
		} else if (p == true) {
			ADD_FLAG("LIBS" + target, libname);
			have = 1;
		} else {
			/* not found in the defaults or the explicit paths,
			 * so check the general extra libs; if we find
			 * it here, no need to add another /libpath: for it as we
			 * already have it covered, but we need to add the lib
			 * to LIBS_XXX */
			if (false != search_paths(libname, PHP_EXTRA_LIBS, null)) {
				ADD_FLAG("LIBS" + target, libname);
				have = 1;
			}
		}

		if (have) {
			break;
		}
	}

//	AC_DEFINE("HAVE_" + header_name.toUpperCase().replace(new RegExp("/\\\\-\.", "g"), "_"), have);

	return have;

}

function CHECK_FUNC_IN_HEADER(header_name, func_name, path_to_check, add_to_flag)
{
	var c = false;
	var sym;

	STDOUT.Write("Checking for " + func_name + " in " + header_name + " ... ");

	c = GREP_HEADER(header_name, func_name, path_to_check);

	sym = func_name.toUpperCase();
	sym = sym.replace(new RegExp("[\\\\/\.-]", "g"), "_");

	if (typeof(add_to_flag) == "undefined") {
		AC_DEFINE("HAVE_" + sym, c ? 1 : 0);
	} else {
		ADD_FLAG(add_to_flag, "/DHAVE_" + sym + "=" + (c ? "1" : "0"));
	}

	if (c) {
		STDOUT.WriteLine("OK");
		return c;
	}
	STDOUT.WriteLine("No");
	return false;	
}

function GREP_HEADER(header_name, regex, path_to_check)
{
	var c = false;

	if (FSO.FileExists(path_to_check + "\\" + header_name)) {
		c = file_get_contents(path_to_check + "\\" + header_name);
	}

	if (!c) {
		/* look in the include path */

		var p = search_paths(header_name, path_to_check, "INCLUDE");
		if (typeof(p) == "string") {
			c = file_get_contents(p);
		} else if (p == false) {
			p = search_paths(header_name, PHP_EXTRA_INCLUDES, null);
			if (typeof(p) == "string") {
				c = file_get_contents(p);
			}
		} 
		if (!c) {
			return false;
		}
	}

	if (typeof(regex) == "string") {
		regex = new RegExp(regex);
	}

	if (c.match(regex)) {
		/* caller can now use RegExp.$1 etc. to get at patterns */
		return true;
	}
	return false;
}

function CHECK_HEADER_ADD_INCLUDE(header_name, flag_name, path_to_check, use_env, add_dir_part, add_to_flag_only)
{
	var dir_part_to_add = "";
	
	if (use_env == null) {
		use_env = true;
	}

	// if true, add the dir part of the header_name to the include path
	if (add_dir_part == null) {
		add_dir_part = false;
	} else if (add_dir_part) {
		var basename = FSO.GetFileName(header_name);
		dir_part_to_add = "\\" + header_name.substr(0, header_name.length - basename.length - 1);
	}

	if (path_to_check == null) {
		path_to_check = php_usual_include_suspects;
	} else {
		path_to_check += ";" + php_usual_include_suspects;
	}
	
	var p = search_paths(header_name, path_to_check, use_env ? "INCLUDE" : null);
	var have = 0;
	var sym;

	if (typeof(p) == "string") {
		ADD_FLAG(flag_name, '/I "' + p + dir_part_to_add + '" ');
	} else if (p == false) {
		/* not found in the defaults or the explicit paths,
		 * so check the general extra includes; if we find
		 * it here, no need to add another /I for it as we
		 * already have it covered, unless we are adding
		 * the dir part.... */
		p = search_paths(header_name, PHP_EXTRA_INCLUDES, null);
		if (typeof(p) == "string" && add_dir_part) {
			ADD_FLAG(flag_name, '/I "' + p + dir_part_to_add + '" ');
		}
	} 
	have = p ? 1 : 0

	sym = header_name.toUpperCase();
	sym = sym.replace(new RegExp("[\\\\/\.-]", "g"), "_");

	if (typeof(add_to_flag_only) == "undefined" &&
			flag_name.match(new RegExp("^CFLAGS_(.*)$"))) {
		add_to_flag_only = true;
	}

	if (typeof(add_to_flag_only) != "undefined") {
		ADD_FLAG(flag_name, "/DHAVE_" + sym + "=" + have);
	} else {
		AC_DEFINE("HAVE_" + sym, have, "have the " + header_name + " header file");
	}

	return p;
}

/* emits rule to generate version info for a SAPI
 * or extension.  Returns the name of the .res file
 * that will be generated */
function generate_version_info_resource(makefiletarget, basename, creditspath, sapi)
{
	var resname = makefiletarget + ".res";
	var res_desc = makefiletarget;
	var res_prod_name = "PHP " + makefiletarget;
	var credits;
	var thanks = "";
	var logo = "";
	var debug = "";
	var project_url = "http://www.php.net";
	var project_header = creditspath + "/php_" + basename + ".h";
	var versioning = "";

	if (sapi) {
		var internal_name = basename.toUpperCase() + " SAPI";
	} else {
		var internal_name = basename.toUpperCase() + " extension";
	}

	if (FSO.FileExists(creditspath + '/CREDITS')) {
		credits = FSO.OpenTextFile(creditspath + '/CREDITS', 1);
		res_desc = credits.ReadLine();
		try {
			thanks = credits.ReadLine();
		} catch (e) {
			thanks = null;
		}
		if (thanks == null) {
			thanks = "";
		} else {
			thanks = "Thanks to " + thanks;
		}
		credits.Close();
	}

	if (creditspath.match(new RegExp("pecl"))) {
		/* PECL project url - this will eventually work correctly for all */
		project_url = "http://pecl.php.net/" + basename;

		/* keep independent versioning PECL-specific for now */
		if (FSO.FileExists(project_header)) {
			if (header = FSO.OpenTextFile(project_header, 1)) {
				contents = header.ReadAll();
				/* allowed: x.x.x[a|b|-alpha|-beta][RCx][-dev] */
				if (contents.match(new RegExp('PHP_' + basename.toUpperCase() + '_VERSION(\\s+)"((\\d+\.\\d+(\.\\d+)?)((a|b)(\\d)?|\-[a-z]{3,5})?(RC\\d+)?(\-dev)?)'))) {
					project_version = RegExp.$2;
					file_version = RegExp.$3.split('.');
					if (!file_version[2]) {
						file_version[2] = 0;
					}
					versioning = '\\"" /d EXT_FILE_VERSION=' + file_version[0] + ',' + file_version[1] + ',' + file_version[2] + ' /d EXT_VERSION="\\"' + project_version;
				}
				header.Close();
			}
		}
	}

	if (makefiletarget.match(new RegExp("\\.exe$"))) {
		logo = " /d WANT_LOGO ";
	}

	if (PHP_DEBUG != "no") {
		debug = " /d _DEBUG";
	}

	/**
	 * Use user supplied template.rc if it exists
	 */
	if (FSO.FileExists(creditspath + '\\template.rc')) {
		MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": " + creditspath + "\\template.rc");
		MFO.WriteLine("\t$(RC) /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"' +
			makefiletarget + '\\"" /d PRODUCT_NAME="\\"' + res_prod_name +
			versioning + '\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" ' +
			creditspath + '\\template.rc');
		return resname;
	}
	if (MODE_PHPIZE) {
		MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": $(PHP_DIR)\\build\\template.rc");
		MFO.WriteLine("\t$(RC)  /I $(PHP_DIR)/include /n /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"'
			+ makefiletarget + '\\"" /d URL="\\"' + project_url + 
			'\\"" /d INTERNAL_NAME="\\"' + internal_name + versioning + 
			'\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" $(PHP_DIR)\\build\\template.rc');
	} else {
		MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": win32\\build\\template.rc");
		MFO.WriteLine("\t$(RC) /n /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"'
			+ makefiletarget + '\\"" /d URL="\\"' + project_url + 
			'\\"" /d INTERNAL_NAME="\\"' + internal_name + versioning + 
			'\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" win32\\build\\template.rc');
	}
	MFO.WriteBlankLines(1);
	return resname;
}

function SAPI(sapiname, file_list, makefiletarget, cflags, obj_dir)
{
	var SAPI = sapiname.toUpperCase();
	var ldflags;
	var resname;
	var ld;
	var manifest;

	if (typeof(obj_dir) == "undefined") {
		sapiname_for_printing = configure_module_dirname;
	} else {
		sapiname_for_printing = configure_module_dirname + " (via " + obj_dir + ")";
	}

	STDOUT.WriteLine("Enabling SAPI " + sapiname_for_printing);

	MFO.WriteBlankLines(1);
	MFO.WriteLine("# objects for SAPI " + sapiname);
	MFO.WriteBlankLines(1);

	if (cflags) {
		ADD_FLAG('CFLAGS_' + SAPI, cflags);
	}

	ADD_SOURCES(configure_module_dirname, file_list, sapiname, obj_dir);
	MFO.WriteBlankLines(1);
	MFO.WriteLine("# SAPI " + sapiname);
	MFO.WriteBlankLines(1);

	/* generate a .res file containing version information */
	resname = generate_version_info_resource(makefiletarget, sapiname, configure_module_dirname, true);
	
	MFO.WriteLine(makefiletarget + ": $(BUILD_DIR)\\" + makefiletarget);
	MFO.WriteLine("\t@echo SAPI " + sapiname_for_printing + " build complete");
	if (MODE_PHPIZE) {
		MFO.WriteLine("$(BUILD_DIR)\\" + makefiletarget + ": $(DEPS_" + SAPI + ") $(" + SAPI + "_GLOBAL_OBJS) $(PHPLIB) $(BUILD_DIR)\\" + resname);
	} else {
		MFO.WriteLine("$(BUILD_DIR)\\" + makefiletarget + ": $(DEPS_" + SAPI + ") $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname);
	}

	if (makefiletarget.match(new RegExp("\\.dll$"))) {
		ldflags = "/dll $(LDFLAGS)";
		manifest = "-@$(_VC_MANIFEST_EMBED_DLL)";
	} else if (makefiletarget.match(new RegExp("\\.lib$"))) {
		ldflags = "$(LDFLAGS)";
		ld = "$(MAKE_LIB)";
	} else {
		ldflags = "$(LDFLAGS)";
		manifest = "-@$(_VC_MANIFEST_EMBED_EXE)";
	}

	if (MODE_PHPIZE) {
		if (ld) {
			MFO.WriteLine("\t" + ld + " /nologo /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(" + SAPI + "_GLOBAL_OBJS) $(PHPLIB) $(LDFLAGS_" + SAPI + ") $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname);
		} else {
			ld = "@$(CC)";
			MFO.WriteLine("\t" + ld + " /nologo " + " $(" + SAPI + "_GLOBAL_OBJS) $(PHPLIB) $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname + " /link /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(LDFLAGS_" + SAPI + ")");
		}
	} else {
		if (ld) {
			MFO.WriteLine("\t" + ld + " /nologo /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(LDFLAGS_" + SAPI + ") $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname);
		} else {
			ld = "@$(CC)";
			MFO.WriteLine("\t" + ld + " /nologo " + " $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname + " /link /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(LDFLAGS_" + SAPI + ")");
		}
	}

	if (manifest) {
		MFO.WriteLine("\t" + manifest);
	}
		
	DEFINE('CFLAGS_' + SAPI + '_OBJ', '$(CFLAGS_' + SAPI + ')');

	if (configure_module_dirname.match("pecl")) {
		ADD_FLAG("PECL_TARGETS", makefiletarget);
	} else {
		ADD_FLAG("SAPI_TARGETS", makefiletarget);
	}

	if (PHP_DSP != "no") {
		generate_dsp_file(sapiname, configure_module_dirname, file_list, false);
	}

	MFO.WriteBlankLines(1);
	sapi_enabled[sapi_enabled.length] = [sapiname];
}

function ADD_DIST_FILE(filename)
{
	if (configure_module_dirname.match("pecl")) {
		ADD_FLAG("PECL_EXTRA_DIST_FILES", filename);
	} else {
		ADD_FLAG("PHP_EXTRA_DIST_FILES", filename);
	}
}	

function file_get_contents(filename)
{
	var f, c;
	try {
		f = FSO.OpenTextFile(filename, 1);
		c = f.ReadAll();
		f.Close();
		return c;
	} catch (e) {
		STDOUT.WriteLine("Problem reading " + filename);
		return false;
	}
}

// Add a dependency on another extension, so that
// the dependencies are built before extname
function ADD_EXTENSION_DEP(extname, dependson, optional)
{
	var EXT = extname.toUpperCase();
	var DEP = dependson.toUpperCase();
	var dep_present = false;
	var dep_shared = false;

	if (MODE_PHPIZE) {
		ext_deps_js = file_get_contents(PHP_DIR + "\\script\\ext_deps.js");
		eval(ext_deps_js);
	}

	try {
		dep_present = eval("PHP_" + DEP);

		if (dep_present != "no") {
			try {
				dep_shared = eval("PHP_" + DEP + "_SHARED");
			} catch (e) {
				dep_shared = false;
			}
		}

	} catch (e) {
		dep_present = "no";
	}

	if (optional) {
		if (dep_present == "no") {
			MESSAGE("\t" + dependson + " not found: " + dependson + " support in " + extname + " disabled");
			return false;
		}
	}

	var ext_shared = eval("PHP_" + EXT + "_SHARED");

	if (dep_shared) {
		if (!ext_shared) {
			if (optional) {
				MESSAGE("\tstatic " + extname + " cannot depend on shared " + dependson + ": " + dependson + "support disabled");
				return false;
			}
			ERROR("static " + extname + " cannot depend on shared " + dependson);
		}

		ADD_FLAG("LDFLAGS_" + EXT, "/libpath:$(BUILD_DIR)");
		ADD_FLAG("LIBS_" + EXT, "php_" + dependson + ".lib");
		ADD_FLAG("DEPS_" + EXT, "$(BUILD_DIR)\\php_" + dependson + ".lib");

	} else {

		if (dep_present == "no") {
			if (ext_shared) {
				WARNING(extname + " cannot be built: missing dependency, " + dependson + " not found");

				var dllname = ' php_' + extname + '.dll';

				if (!REMOVE_TARGET(dllname, 'EXT_TARGETS')) {
					REMOVE_TARGET(dllname, 'PECL_TARGETS');
				}

				return false;

			}

			ERROR("Cannot build " + extname + "; " + dependson + " not enabled");
			return false;
		}
	} // dependency is statically built-in to PHP
	return true;
}

function EXTENSION(extname, file_list, shared, cflags, dllname, obj_dir)
{
	var objs = null;
	var EXT = extname.toUpperCase();
	var extname_for_printing;

	if (shared == null) {
		eval("shared = PHP_" + EXT + "_SHARED;");
	} else {
		eval("PHP_" + EXT + "_SHARED = shared;");
	}

	if (cflags == null) {
		cflags = "";
	}

	if (typeof(obj_dir) == "undefined") {
		extname_for_printing = configure_module_dirname;
	} else {
		extname_for_printing = configure_module_dirname + " (via " + obj_dir + ")";
	}

	if (shared) {
		STDOUT.WriteLine("Enabling extension " + extname_for_printing + " [shared]");
		cflags = "/D COMPILE_DL_" + EXT + " /D " + EXT + "_EXPORTS=1 " + cflags;
		ADD_FLAG("CFLAGS_PHP", "/D COMPILE_DL_" + EXT);
	} else {
		STDOUT.WriteLine("Enabling extension " + extname_for_printing);
	}

	MFO.WriteBlankLines(1);
	MFO.WriteLine("# objects for EXT " + extname);
	MFO.WriteBlankLines(1);


	ADD_SOURCES(configure_module_dirname, file_list, extname, obj_dir);
	
	MFO.WriteBlankLines(1);

	if (shared) {
		if (dllname == null) {
			dllname = "php_" + extname + ".dll";
		}
		var libname = dllname.substring(0, dllname.length-4) + ".lib";

		var resname = generate_version_info_resource(dllname, extname, configure_module_dirname, false);
		var ld = "@$(CC)";

		MFO.WriteLine("$(BUILD_DIR)\\" + libname + ": $(BUILD_DIR)\\" + dllname);
		MFO.WriteBlankLines(1);
		if (MODE_PHPIZE) {
			MFO.WriteLine("$(BUILD_DIR)\\" + dllname + ": $(DEPS_" + EXT + ") $(" + EXT + "_GLOBAL_OBJS) $(PHPLIB) $(BUILD_DIR)\\" + resname);
			MFO.WriteLine("\t" + ld + " $(" + EXT + "_GLOBAL_OBJS) $(PHPLIB) $(LIBS_" + EXT + ") $(LIBS) $(BUILD_DIR)\\" + resname + " /link /out:$(BUILD_DIR)\\" + dllname + " $(DLL_LDFLAGS) $(LDFLAGS) $(LDFLAGS_" + EXT + ")");
		} else {
			MFO.WriteLine("$(BUILD_DIR)\\" + dllname + ": $(DEPS_" + EXT + ") $(" + EXT + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname);
			MFO.WriteLine("\t" + ld + " $(" + EXT + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(LIBS_" + EXT + ") $(LIBS) $(BUILD_DIR)\\" + resname + " /link /out:$(BUILD_DIR)\\" + dllname + " $(DLL_LDFLAGS) $(LDFLAGS) $(LDFLAGS_" + EXT + ")");
		}
		MFO.WriteLine("\t-@$(_VC_MANIFEST_EMBED_DLL)");
		MFO.WriteBlankLines(1);

		if (configure_module_dirname.match("pecl")) {
			ADD_FLAG("PECL_TARGETS", dllname);
		} else {
			ADD_FLAG("EXT_TARGETS", dllname);
		}
		MFO.WriteLine(dllname + ": $(BUILD_DIR)\\" + dllname);
		MFO.WriteLine("\t@echo EXT " + extname + " build complete");
		MFO.WriteBlankLines(1);
		
		DEFINE('CFLAGS_' + EXT + '_OBJ', '$(CFLAGS_' + EXT + ')');
	} else {
		ADD_FLAG("STATIC_EXT_OBJS", "$(" + EXT + "_GLOBAL_OBJS)");
		ADD_FLAG("STATIC_EXT_LIBS", "$(LIBS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_LDFLAGS", "$(LDFLAGS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_CFLAGS", "$(CFLAGS_" + EXT + ")");

		/* find the header that declares the module pointer,
		 * so we can include it in internal_functions.c */
		var ext_dir = FSO.GetFolder(configure_module_dirname);
		var fc = new Enumerator(ext_dir.Files);
		var re = /\.h$/;
		var s, c;
		for (; !fc.atEnd(); fc.moveNext()) {
			s = fc.item() + "";
			if (s.match(re)) {
				c = file_get_contents(s);
				if (c.match("phpext_")) {
					extension_include_code += '#include "' + configure_module_dirname + '/' + FSO.GetFileName(s) + '"\r\n';
				}
			}
		}
	
		extension_module_ptrs += '\tphpext_' + extname + '_ptr,\r\n';
	
		DEFINE('CFLAGS_' + EXT + '_OBJ', '$(CFLAGS_PHP) $(CFLAGS_' + EXT + ')');
	}
	ADD_FLAG("CFLAGS_" + EXT, cflags);

	if (PHP_DSP != "no") {
		generate_dsp_file(extname, configure_module_dirname, file_list, shared);
	}

	extensions_enabled[extensions_enabled.length] = [extname, shared ? 'shared' : 'static'];
}

function ADD_SOURCES(dir, file_list, target, obj_dir)
{
	var i;
	var tv;
	var src, obj, sym, flags;

	if (target == null) {
		target = "php";
	}

	sym = target.toUpperCase() + "_GLOBAL_OBJS";
	flags = "CFLAGS_" + target.toUpperCase() + '_OBJ';

	if (configure_subst.Exists(sym)) {
		tv = configure_subst.Item(sym);
	} else {
		tv = "";
	}

	file_list = file_list.split(new RegExp("\\s+"));
	file_list.sort();

	var re = new RegExp("\.[a-z0-9A-Z]+$");

	dir = dir.replace(new RegExp("/", "g"), "\\");
	var objs_line = "";
	var srcs_line = "";

	var sub_build = "$(BUILD_DIR)\\";

	/* if module dir is not a child of the main source dir,
	 * we need to tweak it; we should have detected such a
	 * case in condense_path and rewritten the path to
	 * be relative.
	 * This probably breaks for non-sibling dirs, but that
	 * is not a problem as buildconf only checks for pecl
	 * as either a child or a sibling */
	if (obj_dir == null) {
		var build_dir = dir.replace(new RegExp("^..\\\\"), "");
		var mangle_dir = build_dir.replace(new RegExp("[\\\\/.-]", "g"), "_");
		var bd_flags_name = "CFLAGS_BD_" + mangle_dir.toUpperCase();
	}
	else {
		var build_dir = obj_dir.replace(new RegExp("^..\\\\"), "");
		var mangle_dir = build_dir.replace(new RegExp("[\\\\/.-]", "g"), "_");
		var bd_flags_name = "CFLAGS_BD_" + mangle_dir.toUpperCase();
	}
	
	var dirs = build_dir.split("\\");
	var i, d = "";
	for (i = 0; i < dirs.length; i++) {
		d += dirs[i];
		build_dirs[build_dirs.length] = d;
		d += "\\";
	}
	sub_build += d;


	DEFINE(bd_flags_name, " /Fd" + sub_build + " /Fp" + sub_build + " /FR" + sub_build + " ");

	for (i in file_list) {
		src = file_list[i];
		obj = src.replace(re, ".obj");
		tv += " " + sub_build + obj;

		if (!MODE_PHPIZE && PHP_ONE_SHOT == "yes") {
			if (i > 0) {
				objs_line += " " + sub_build + obj;	
				srcs_line += " " + dir + "\\" + src;
			} else {
				objs_line = sub_build + obj;	
				srcs_line = dir + "\\" + src;
			}
		} else {
			MFO.WriteLine(sub_build + obj + ": " + dir + "\\" + src);
			MFO.WriteLine("\t@$(CC) $(" + flags + ") $(CFLAGS) $(" + bd_flags_name + ") /c " + dir + "\\" + src + " /Fo" + sub_build + obj);
		}
	}

	if (!MODE_PHPIZE && PHP_ONE_SHOT == "yes") {
		MFO.WriteLine(objs_line + ": " + srcs_line);
		MFO.WriteLine("\t$(CC) $(" + flags + ") $(CFLAGS) /Fo" + sub_build + " $(" + bd_flags_name + ") /c " + srcs_line);
	}

	DEFINE(sym, tv);
}

function REMOVE_TARGET(dllname, flag)
{
	var dllname = dllname.replace(/\s/g, "");
	var EXT = dllname.replace(/php_(\S+)\.dll/, "$1").toUpperCase();
	var php_flags = configure_subst.Item("CFLAGS_PHP");

	if (configure_subst.Exists(flag)) {
		var targets = configure_subst.Item(flag);

		if (targets.match(dllname)) {
			configure_subst.Remove(flag);
			targets = targets.replace(dllname, "");
			targets = targets.replace(/\s+/, " ");
			targets = targets.replace(/\s$/, "");
			configure_subst.Add(flag, targets);
			configure_hdr.Add("HAVE_" + EXT, new Array(0, ""));
			configure_subst.Item("CFLAGS_PHP") = php_flags.replace(" /D COMPILE_DL_" + EXT, "");
			extensions_enabled.pop();
			return true;
		}
	}
	return false;
}

function generate_internal_functions()
{
	var infile, outfile;
	var indata;

	STDOUT.WriteLine("Generating main/internal_functions.c");
	
	infile = FSO.OpenTextFile("main/internal_functions.c.in", 1);
	indata = infile.ReadAll();
	infile.Close();
	
	indata = indata.replace("@EXT_INCLUDE_CODE@", extension_include_code);
	indata = indata.replace("@EXT_MODULE_PTRS@", extension_module_ptrs);

	if (FSO.FileExists("main/internal_functions.c")) {
		var origdata = file_get_contents("main/internal_functions.c");

		if (origdata == indata) {
			STDOUT.WriteLine("\t[content unchanged; skipping]");
			return;
		}
	}

	outfile = FSO.CreateTextFile("main/internal_functions.c", true);
	outfile.Write(indata);
	outfile.Close();
}

function output_as_table(header, ar_out)
{
	var l = header.length;
	var cols = 80;
	var fixedlenght = "";
	var t = 0;
	var i,j,k,m;
	var out = "| ";
	var min = new Array(l);
	var max = new Array(l);

	if (l != ar_out[0].length) {
		STDOUT.WriteLine("Invalid header argument, can't output the table " + l + " " + ar_out[0].length  );
		return;
	}

	for (j=0; j < l; j++) {
		var tmax, tmin;

		/*Figure out the max length per column */
		tmin = 0;
		tmax = 0;
		for (k = 0; k < ar_out.length; k++) {
			var t = ar_out[k][j].length;
			if (t > tmax) tmax = t;
			else if (t < tmin) tmin = t;
		}
		if (tmax > header[j].length) {
			max[j] = tmax;
		} else {
			max[j] = header[j].length;
		}
		if (tmin < header[j].length) {
			min[j] = header[j].length;
		}
	}

	sep = "";
	k = 0;
	for (i = 0; i < l; i++) {
		k += max[i] + 3;
	}
	k++;

	for (j=0; j < k; j++) {
		sep += "-";
	}

	STDOUT.WriteLine(sep);
	out = "|";
	for (j=0; j < l; j++) {
		out += " " + header[j];
		for (var i = 0; i < (max[j] - header[j].length); i++){
			out += " ";
		}
		out += " |";
	}
	STDOUT.WriteLine(out);

	STDOUT.WriteLine(sep);

	out = "|";
	for (i=0; i < ar_out.length; i++) {
		line = ar_out[i];
		for (j=0; j < l; j++) {
			out += " " + line[j];
			for (var k = 0; k < (max[j] - line[j].length); k++){
				out += " ";
			}
			out += " |";
		}
		STDOUT.WriteLine(out);
		out = "|";
	}

	STDOUT.WriteLine(sep);
}

function write_summary()
{
	var ar = new Array();

	STDOUT.WriteBlankLines(2);

	STDOUT.WriteLine("Enabled extensions:");
	output_as_table(["Extension", "Mode"], extensions_enabled.sort());
	STDOUT.WriteBlankLines(2);
	if (!MODE_PHPIZE) {
		STDOUT.WriteLine("Enabled SAPI:");
		output_as_table(["Sapi Name"], sapi_enabled);
		STDOUT.WriteBlankLines(2);
	}
	ar[0] = ['Build type', PHP_DEBUG == "yes" ? "Debug" : "Release"];
	ar[1] = ['Thread Safety', PHP_ZTS == "yes" ? "Yes" : "No"];
	ar[2] = ['Compiler', VC_VERSIONS[VCVERS]];
	ar[3] = ['Architecture', X64 ? 'x64' : 'x86'];

	output_as_table(["",""], ar);
	STDOUT.WriteBlankLines(2);
}

function generate_files()
{
	var i, dir, bd, last;

	STDOUT.WriteBlankLines(1);
	STDOUT.WriteLine("Creating build dirs...");
	dir = get_define("BUILD_DIR");
	build_dirs.sort();
	last = null;

	if (!FSO.FolderExists(dir)) {
		FSO.CreateFolder(dir);
	}

	for (i = 0; i < build_dirs.length; i++) {
		bd = FSO.BuildPath(dir, build_dirs[i]);
		if (bd == last) {
			continue;
		}
		last = bd;

		build_dir = get_define('BUILD_DIR');
		build_dir = build_dir.replace(new RegExp("\\\\", "g"), "\\\\");
		if (build_dir.substr(build_dir.Length - 2, 2) != '\\\\') {
			build_dir += '\\\\';
		}
		ADD_FLAG("BUILD_DIRS_SUB", bd.replace(new RegExp(build_dir), ''));

		if (!FSO.FolderExists(bd)) {
			FSO.CreateFolder(bd);
		}
	}

	if (PHP_DSP != "no") {
		generate_dsp_file("TSRM", "TSRM", null, false);
		generate_dsp_file("Zend", "Zend", null, false);
		generate_dsp_file("win32", "win32", null, false);
		generate_dsp_file("main", "main", null, false);
		generate_dsp_file("streams", "main\\streams", null, false);
		copy_dsp_files();
	}

	STDOUT.WriteLine("Generating files...");
	generate_makefile();
	if (!MODE_PHPIZE) {
		generate_internal_functions();
		generate_config_h();
		generate_phpize();
	}
	STDOUT.WriteLine("Done.");
	STDOUT.WriteBlankLines(1);
	write_summary();

	if (PHP_SNAPSHOT_BUILD != "no") {
		STDOUT.WriteLine("Type 'nmake snap' to build a PHP snapshot");
	} else {
		STDOUT.WriteLine("Type 'nmake' to build PHP");
	}
}

function generate_config_h()
{
	var infile, outfile;
	var indata;
	var prefix;

	prefix = PHP_PREFIX.replace(new RegExp("\\\\", "g"), "\\\\");

	STDOUT.WriteLine("Generating main/config.w32.h");
	
	infile = FSO.OpenTextFile("win32/build/config.w32.h.in", 1);
	indata = infile.ReadAll();
	infile.Close();
	
	outfile = FSO.CreateTextFile("main/config.w32.h", true);

	indata = indata.replace(new RegExp("@PREFIX@", "g"), prefix);
	outfile.Write(indata);

	var keys = (new VBArray(configure_hdr.Keys())).toArray();
	var i, j;
	var item;
	var pieces, stuff_to_crack, chunk;

	outfile.WriteBlankLines(1);
	outfile.WriteLine("/* values determined by configure.js */");

	for (i in keys) {
		item = configure_hdr.Item(keys[i]);
		outfile.WriteBlankLines(1);
		pieces = item[0];

		if (item[1] != undefined) {
			outfile.WriteLine("/* " + item[1] + " */");
		}

		if (typeof(pieces) == "string" && pieces.charCodeAt(0) == 34) {
			/* quoted string have a maximal length of 2k under vc.
			 * solution is to crack them and let the compiler concat
			 * them implicitly */
			stuff_to_crack = pieces;
			pieces = "";

			while (stuff_to_crack.length) {
				j = 65;
				while (stuff_to_crack.charCodeAt(j) != 32 && j < stuff_to_crack.length)
					j++;

				chunk = stuff_to_crack.substr(0, j);
				pieces += chunk;
				stuff_to_crack = stuff_to_crack.substr(chunk.length);
				if (stuff_to_crack.length)
					pieces += '" "';
			}
		}
		
		outfile.WriteLine("#define " + keys[i] + " " + pieces);
	}
	
	outfile.Close();
}

function generate_phpize()
{
	STDOUT.WriteLine("Generating phpize");
	dest = get_define("BUILD_DIR") + '/devel';

	if (!FSO.FolderExists(dest)) {
		FSO.CreateFolder(dest);
	}

	var MF = FSO.CreateTextFile(dest + "/phpize.js", true);
	var DEPS = FSO.CreateTextFile(dest + "/ext_deps.js", true);
	prefix = get_define("PHP_PREFIX");
	prefix = prefix.replace(new RegExp("/", "g"), "\\");
	prefix = prefix.replace(new RegExp("\\\\", "g"), "\\\\");
	MF.WriteLine("var PHP_PREFIX=" + '"' + prefix + '"');
	MF.WriteLine("var PHP_ZTS=" + '"' + (PHP_ZTS.toLowerCase() == "yes" ? "Yes" : "No") + '"');
	MF.WriteLine("var VC_VERSION=" + VCVERS);
	MF.WriteLine("var PHP_VERSION=" + PHP_VERSION);
	MF.WriteLine("var PHP_MINOR_VERSION=" + PHP_MINOR_VERSION);
	MF.WriteLine("var PHP_RELEASE_VERSION=" + PHP_RELEASE_VERSION);
	MF.WriteBlankLines(1);
	MF.WriteLine("/* Genereted extensions list with mode (static/shared) */");

	var count = extensions_enabled.length;
	for (i in extensions_enabled) {
		out = "PHP_" + extensions_enabled[i][0].toUpperCase() + "_SHARED=" + (extensions_enabled[i][1] == 'shared' ? 'true' : 'false') + ";";
		DEPS.WriteLine("PHP_" + extensions_enabled[i][0].toUpperCase() + "=true;");
		DEPS.WriteLine(out);
		MF.WriteLine(out);
	}

	MF.WriteBlankLines(2);
	MF.WriteLine("/* Genereted win32/build/phpize.js.in */");
	MF.WriteBlankLines(1);
	MF.Write(file_get_contents("win32/build/phpize.js.in"));
	MF.Close();
	DEPS.Close();

	/* Generate flags file */
	/* spit out variable definitions */
	CJ = FSO.CreateTextFile(dest + "/config.phpize.js");

	CJ.WriteLine("var PHP_ZTS =" + '"' + PHP_ZTS + '"');
	CJ.WriteLine("var PHP_DLL_LIB =" + '"' + get_define('PHPLIB') + '"');
	CJ.WriteLine("var PHP_DLL =" + '"' + get_define('PHPDLL') + '"');
	CJ.WriteBlankLines(1);
	CJ.Close();
}

function generate_makefile()
{
	STDOUT.WriteLine("Generating Makefile");
	var MF = FSO.CreateTextFile("Makefile", true);

	MF.WriteLine("# Generated by configure.js");
	/* spit out variable definitions */
	var keys = (new VBArray(configure_subst.Keys())).toArray();
	var i;
	MF.WriteLine("PHP_SRC_DIR =" + PHP_SRC_DIR);
	for (i in keys) {
		// The trailing space is needed to prevent the trailing backslash
		// that is part of the build dir flags (CFLAGS_BD_XXX) from being
		// seen as a line continuation character
		MF.WriteLine(keys[i] + "=" + 
			//word_wrap_and_indent(1, configure_subst.Item(keys[i]), ' \\', '\t') + " "
			configure_subst.Item(keys[i]) + " "
			);
		MF.WriteBlankLines(1);
	}

	MF.WriteBlankLines(1);
	if (MODE_PHPIZE) {
		var TF = FSO.OpenTextFile(PHP_DIR + "/script/Makefile.phpize", 1);
	} else {
		var TF = FSO.OpenTextFile("win32/build/Makefile", 1);
	}

	MF.Write(TF.ReadAll());

	MF.WriteLine("build-headers:");
	MF.WriteLine("	@if not exist $(BUILD_DIR_DEV)\\include mkdir $(BUILD_DIR_DEV)\\include >nul");
	MF.WriteLine("	@for %D in ($(INSTALL_HEADERS_DIR)) do @if not exist $(BUILD_DIR_DEV)\\include\\%D mkdir $(BUILD_DIR_DEV)\\include\\%D >nul");
	for (i in headers_install) {
		if (headers_install[i][2] != "") {
				MF.WriteLine("	@if not exist $(BUILD_DIR_DEV)\\include\\" + headers_install[i][2] + " mkdir $(BUILD_DIR_DEV)\\include\\" + 
												headers_install[i][2] + ">nul");
				MF.WriteLine("	@copy " + headers_install[i][0] + " " + "$(BUILD_DIR_DEV)\\include\\" + headers_install[i][2] + " /y >nul");
		}
	}
	MF.WriteLine("	@for %D in ($(INSTALL_HEADERS_DIR)) do @copy %D*.h $(BUILD_DIR_DEV)\\include\\%D /y >nul");
	TF.Close();

	MF.WriteBlankLines(2);

	MFO.Close();
	TF = FSO.OpenTextFile("Makefile.objects", 1);
	MF.Write(TF.ReadAll());
	TF.Close();

	MF.Close();	
}

function ADD_FLAG(name, flags, target)
{
	if (target != null) {
		name = target.toUpperCase() + "_" + name;
	}
	if (configure_subst.Exists(name)) {
		var curr_flags = configure_subst.Item(name);

		if (curr_flags.indexOf(flags) >= 0) {
			return;
		}
		
		flags = curr_flags + " " + flags;
		configure_subst.Remove(name);
	}
	configure_subst.Add(name, flags);

	if (PHP_DSP != "no") {
		if (flags && (name.substr(name.length-3) != "PHP") && (name.substr(0, 7) == "CFLAGS_")) {
			DSP_FLAGS[DSP_FLAGS.length] = new Array(name, flags);
		}
	}
}

function get_define(name)
{
	if (configure_subst.Exists(name)) {
		return configure_subst.Item(name);
	}
	return "";
}

// Add a .def to the core to export symbols
function ADD_DEF_FILE(name)
{
	if (!configure_subst.Exists("PHPDEF")) {
		DEFINE("PHPDEF", "$(BUILD_DIR)\\$(PHPDLL).def");
		ADD_FLAG("PHP_LDFLAGS", "/def:$(PHPDEF)");
	}
	ADD_FLAG("PHP_DLL_DEF_SOURCES", name);
}

function AC_DEFINE(name, value, comment, quote)
{
	if (quote == null) {
		quote = true;
	}
	if (quote && typeof(value) == "string") {
		value = '"' + value.replace(new RegExp('(["\\\\])', "g"), '\\$1') + '"';
	} else if (value.length == 0) {
		value = '""';
	}
	var item = new Array(value, comment);
	if (configure_hdr.Exists(name)) {
		var orig_item = configure_hdr.Item(name);
		STDOUT.WriteLine("AC_DEFINE[" + name + "]=" + value + ": is already defined to " + orig_item[0]);
	} else {
		configure_hdr.Add(name, item);
	}
}

function MESSAGE(msg)
{
	STDOUT.WriteLine("" + msg);
}

function ERROR(msg)
{
	STDERR.WriteLine("ERROR: " + msg);
	WScript.Quit(3);
}

function WARNING(msg)
{
	STDERR.WriteLine("WARNING: " + msg);
	STDERR.WriteBlankLines(1);
}

function copy_and_subst(srcname, destname, subst_array)
{
	if (!FSO.FileExists(srcname)) {
		srcname = configure_module_dirname + "\\" + srcname;
		destname = configure_module_dirname + "\\" + destname;
	}

	var content = file_get_contents(srcname);
	var i;

	for (i = 0; i < subst_array.length; i+=2) {
		var re = subst_array[i];
		var rep = subst_array[i+1];

		content = content.replace(re, rep);
	}
	
	var f = FSO.CreateTextFile(destname, true);
	f.Write(content);
	f.Close();
}

// glob using simple filename wildcards
// returns an array of matches that are found
// in the filesystem
function glob(path_pattern)
{
	var path_parts = path_pattern.replace(new RegExp("/", "g"), "\\").split("\\");
	var p;
	var base = "";
	var is_pat_re = /\*/;

//STDOUT.WriteLine("glob: " + path_pattern);

	if (FSO.FileExists(path_pattern)) {
		return new Array(path_pattern);
	}
	
	// first, build as much as possible that doesn't have a pattern
	for (p = 0; p < path_parts.length; p++) {
		if (path_parts[p].match(is_pat_re))
			break;
		if (p)
			base += "\\";
		base += path_parts[p];	
	}

	return _inner_glob(base, p, path_parts);
}

function _inner_glob(base, p, parts)
{
	var pat = parts[p];
	var full_name = base + "\\" + pat;
	var re = null;
	var items = null;

	if (p == parts.length) {
		return false;
	}

//STDOUT.WriteLine("inner: base=" + base + " p=" + p + " pat=" + pat);

	if (FSO.FileExists(full_name)) {
		if (p < parts.length - 1) {
			// we didn't reach the full extent of the pattern
			return false;
		}
		return new Array(full_name);
	}

	if (FSO.FolderExists(full_name) && p == parts.length - 1) {
		// we have reached the end of the pattern; no need to recurse
		return new Array(full_name);
	}

	// Convert the pattern into a regexp
	re = new RegExp("^" + pat.replace(/\./g, '\\.').replace(/\*/g, '.*').replace(/\?/g, '.') + "$", "i");

	items = new Array();

	if (!FSO.FolderExists(base)) {
		return false;
	}

	var folder = FSO.GetFolder(base);
	var fc = null;
	var subitems = null;
	var item_name = null;
	var j;

	fc = new Enumerator(folder.SubFolders);
	for (; !fc.atEnd(); fc.moveNext()) {
		item_name = FSO.GetFileName(fc.item());

		if (item_name.match(re)) {
			// got a match; if we are at the end of the pattern, just add these
			// things to the items array
			if (p == parts.length - 1) {
				items[items.length] = fc.item();
			} else {
				// we should recurse and do more matches
				subitems = _inner_glob(base + "\\" + item_name, p + 1, parts);
				if (subitems) {
					for (j = 0; j < subitems.length; j++) {
						items[items.length] = subitems[j];
					}
				}
			}
		}
	}

	// if we are at the end of the pattern, we should match
	// files too
	if (p == parts.length - 1) {
		fc = new Enumerator(folder.Files);
		for (; !fc.atEnd(); fc.moveNext()) {
			item_name = FSO.GetFileName(fc.item());
			if (item_name.match(re)) {
				items[items.length] = fc.item();
			}
		}
	}

	if (items.length == 0)
		return false;

	return items;
}

function PHP_INSTALL_HEADERS(dir, headers_list)
{
	headers_list = headers_list.split(new RegExp("\\s+"));
	headers_list.sort();
	if (dir.length > 0 && dir.substr(dir.length - 1) != '/' && dir.substr(dir.length - 1) != '\\') {
		dir += '/';
	}
	dir = dir.replace(new RegExp("/", "g"), "\\");

	for (i in headers_list) {
		found = false;
		src = headers_list[i];
		src = src.replace(new RegExp("/", "g"), "\\");
		isdir = FSO.FolderExists(dir + src);
		isfile = FSO.FileExists(dir + src);
		if (isdir) {
			if (src.length > 0 && src.substr(src.length - 1) != '/' && src.substr(src.length - 1) != '\\') {
				src += '\\';
			}
			headers_install[headers_install.length] = [dir + src, 'dir',''];
			ADD_FLAG("INSTALL_HEADERS_DIR", dir + src);
			found = true;
		} else if (isfile) {
			dirname = FSO.GetParentFolderName(dir + src);
			headers_install[headers_install.length] = [dir + src, 'file', dirname];
			ADD_FLAG("INSTALL_HEADERS", dir + src);
			found = true;
		} else {
			path =  configure_module_dirname + "\\"+ src;
			isdir = FSO.FolderExists(path);
			isfile = FSO.FileExists(path);
			if (isdir) {
				if (src.length > 0 && src.substr(src.length - 1) != '/' && src.substr(src.length - 1) != '\\') {
					src += '\\';
				}
				headers_install[headers_install.length] = [path, 'dir',''];
				ADD_FLAG("INSTALL_HEADERS_DIR", path);
			} else if (isfile) {
				dirname = FSO.GetParentFolderName(path);
				headers_install[headers_install.length] = [path, 'file', dir];
				ADD_FLAG("INSTALL_HEADERS", dir + src);
				found = true;
			}
		}

		if (found == false) {
			STDOUT.WriteLine(headers_list);
			ERROR("Cannot find header " + dir + src);
		}
	}
}

// for snapshot builders, this option will attempt to enable everything
// and you can then build everything, ignoring fatal errors within a module
// by running "nmake snap"
PHP_SNAPSHOT_BUILD = "no";
if (!MODE_PHPIZE) {
	ARG_ENABLE('snapshot-build', 'Build a snapshot; turns on everything it can and ignores build errors', 'no');

	// one-shot build optimizes build by asking compiler to build
	// several objects at once, reducing overhead of starting new
	// compiler processes.
	ARG_ENABLE('one-shot', 'Optimize for fast build - best for release and snapshot builders, not so hot for edit-and-rebuild hacking', 'no');
}


// Utils for configure script
/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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

var STDOUT = WScript.StdOut;
var STDERR = WScript.StdErr;
var WshShell = WScript.CreateObject("WScript.Shell");
var FSO = WScript.CreateObject("Scripting.FileSystemObject");
var MFO = null;
var SYSTEM_DRIVE = WshShell.Environment("Process").Item("SystemDrive");
var PROGRAM_FILES = WshShell.Environment("Process").Item("ProgramFiles");
var PROGRAM_FILESx86 = WshShell.Environment("Process").Item("ProgramFiles(x86)");
var VCINSTALLDIR = WshShell.Environment("Process").Item("VCINSTALLDIR");
var PHP_SRC_DIR=FSO.GetParentFolderName(WScript.ScriptFullName);

var VS_TOOLSET = true;
var CLANG_TOOLSET = false;
var ICC_TOOLSET = false;
var VCVERS = -1;
var CLANGVERS = -1;
var INTELVERS = -1;
var COMPILER_NUMERIC_VERSION = -1;
var COMPILER_NAME_LONG = "unknown";
var COMPILER_NAME_SHORT = "unknown";
var PHP_OBJECT_OUT_DIR = "";
var PHP_CONFIG_PROFILE = "no";
var PHP_SANITIZER = "no";
var VERBOSITY = 0;
var CMD_MOD1 = "@";
var CMD_MOD2 = "@";

var PHP_TEST_INI_PATH = "";
var PHP_TEST_INI = "";
var PHP_TEST_INI_EXT_EXCLUDE = "";

var PHP_MAKEFILE_FRAGMENTS = PHP_SRC_DIR + "\\Makefile.fragments.w32";

/* Care also about NTDDI_VERSION and _WIN32_WINNT in config.w32.h.in
   and manifest. */
var WINVER = "0x0601"; /* 7/2008r2 */

// There's a minimum requirement for bison.
var MINBISON = "3.0.0";

// There's a minimum requirement for re2c..
var MINRE2C = "0.13.4";

/* Store the enabled extensions (summary + QA check) */
var extensions_enabled = new Array();

/* Store the SAPI enabled (summary + QA check) */
var sapi_enabled = new Array();

/* Store the headers to install */
var headers_install = new Array();

/* Store unknown configure options */
var INVALID_CONFIG_ARGS = new Array();

if (PROGRAM_FILES == null) {
	PROGRAM_FILES = "C:\\Program Files";
}

if (MODE_PHPIZE) {
	if (!FSO.FileExists("config.w32")) {
		STDERR.WriteLine("Must be run from the root of the extension source");
		WScript.Quit(10);
	}
} else {
	if (!FSO.FileExists("main\\php_version.h")) {
		STDERR.WriteLine("Must be run from the root of the php source");
		WScript.Quit(10);
	}
}

var CWD = WshShell.CurrentDirectory;

if (typeof(CWD) == "undefined") {
	CWD = FSO.GetParentFolderName(FSO.GetParentFolderName(FSO.GetAbsolutePathName("main\\php_version.h")));
}

if (!MODE_PHPIZE) {
	/* defaults; we pick up the precise versions from configure.ac */
	var PHP_VERSION = 7;
	var PHP_MINOR_VERSION = 4;
	var PHP_RELEASE_VERSION = 0;
	var PHP_EXTRA_VERSION = "";
	var PHP_VERSION_STRING = "7.4.0";
}

/* Get version numbers and DEFINE as a string */
function get_version_numbers()
{
	var cin = file_get_contents("configure.ac");
	var regex = /AC_INIT.+(\d+)\.(\d+)\.(\d+)([^\,^\]]*).+/g;

	if (cin.match(new RegExp(regex))) {
		PHP_VERSION = RegExp.$1;
		PHP_MINOR_VERSION = RegExp.$2;
		PHP_RELEASE_VERSION = RegExp.$3;
		PHP_EXTRA_VERSION = RegExp.$4;
	}

	PHP_VERSION_STRING = PHP_VERSION + "." + PHP_MINOR_VERSION + "." + PHP_RELEASE_VERSION + PHP_EXTRA_VERSION;

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
	} else if (what == "longversion") {
		command = command + '" --version"';
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
	var nice = "cscript /nologo /e:jscript configure.js ";
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
						if (arg.defval == "no") {
							argval = "yes";
						} else if (arg.defval == "no,shared") {
							argval = "yes,shared";
							shared = true;
						} else {
							argval = "no";
						}
					}
				}

				arg.argval = argval;
				eval("PHP_" + arg.symval + " = argval;");
				eval("PHP_" + arg.symval + "_SHARED = shared;");
				break;
			}
		}
		if (!found) {
			INVALID_CONFIG_ARGS[INVALID_CONFIG_ARGS.length] = argname;
		}
	}

	if (PHP_SNAPSHOT_BUILD != 'no' && INVALID_CONFIG_ARGS.length) {
		STDERR.WriteLine('Unknown option ' + INVALID_CONFIG_ARGS[0] + '; please try configure.js --help for a list of valid options');
		WScript.Quit(2);
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
		STDOUT.WriteBlankLines(1);
		STDOUT.WriteLine("Some influential environment variables:");
		STDOUT.WriteLine("  CFLAGS      C compiler flags");
		STDOUT.WriteLine("  LDFLAGS     linker flags");
		WScript.Quit(1);
	}

	var snapshot_build_exclusions = new Array(
		'debug', 'crt-debug', 'lzf-better-compression',
		 'php-build', 'snapshot-template', 'ereg',
		 'pcre-regex', 'fastcgi', 'force-cgi-redirect',
		 'path-info-check', 'zts', 'ipv6', 'memory-limit',
		 'zend-multibyte', 'fd-setsize', 'memory-manager',
		 'pgi', 'pgo', 'all-shared', 'config-profile'
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

	var profile = false;

	if (PHP_CONFIG_PROFILE != 'no') {
		if (PHP_CONFIG_PROFILE.toLowerCase() == 'nice') {
			WARNING('Config profile name cannot be named \'nice\'');
		} else {
			var config_profile = FSO.CreateTextFile('config.' + PHP_CONFIG_PROFILE + '.bat', true);

			config_profile.WriteLine('@echo off');
			config_profile.WriteLine(nice + ' %*');
			config_profile.Close();

			profile = true;
		}
	}

	// Only generate an updated config.nice.bat file if a profile was not used
	if (!profile) {
		STDOUT.WriteLine("Saving configure options to config.nice.bat");

		var nicefile = FSO.CreateTextFile("config.nice.bat", true);
		nicefile.WriteLine('@echo off');
		nicefile.WriteLine(nice +  " %*");
		nicefile.Close();
	}

	AC_DEFINE('CONFIGURE_COMMAND', nice, "Configure line");
}

function DEFINE(name, value)
{
	if (configure_subst.Exists(name)) {
		configure_subst.Remove(name);
	}
	configure_subst.Add(name, value);
}

function verbalize_deps_path(path)
{
	var absolute_path = FSO.GetAbsolutePathName(path);

	if (absolute_path.indexOf(PHP_PHP_BUILD) == 0) {
		absolute_path = absolute_path.substring(PHP_PHP_BUILD.length).split('\\');

		if (absolute_path[1] == 'include' || absolute_path[1] == 'lib') {
			return "<in deps path> " + absolute_path.join('\\');
		}
	}

	return path;
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
		STDOUT.WriteLine(" " + verbalize_deps_path(place));
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

function copy_dep_pdb_into_build_dir(libpath)
{
	var candidate;
	var build_dir = get_define("BUILD_DIR");
	var libdir = FSO.GetParentFolderName(libpath);
	var bindir = FSO.GetAbsolutePathName(libdir + "\\..\\bin");

	var names = [];

	var libname = FSO.GetFileName(libpath);

	/* Within same .lib, everything should be bound to the same .pdb. No check
		for every single object in the static libs. */
	var _tmp = execute("dumpbin /section:.debug$T /rawdata:1,256 " + libpath);
	if (!_tmp.match("LNK4039")) {
		if (_tmp.match(/\d{2,}:\s+([a-z0-9\s]+)/i)) {
			var m = RegExp.$1;
			var a = m.split(/ /);
			var s = "";
			for (var i in a) {
				s = s + String.fromCharCode(parseInt("0x" + a[i]));
			}

			if (s.match(/([^\\]+\.pdb)/i)) {
				if (RegExp.$1 != names[0]) {
					names.push(RegExp.$1);
				}
			}
		}
	}

	/* This is rather a fallback, if the bin has no debug section or
		something went wrong with parsing. */
	names.push(libname.replace(new RegExp("\\.lib$"), ".pdb"));

	for (var k = 0; k < names.length; k++) {
		var pdbname = names[k];

		candidate = bindir + "\\" + pdbname;
		if (FSO.FileExists(candidate)) {
			FSO.CopyFile(candidate, build_dir + "\\" + pdbname, true);
			return true;
		}

		candidate = libdir + "\\" + pdbname;
		if (FSO.FileExists(candidate)) {
			FSO.CopyFile(candidate, build_dir + "\\" + pdbname, true);
			return true;
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
			ADD_FLAG("ARFLAGS" + target, '/libpath:"' + libdir + '" ');
			ADD_FLAG("LIBS" + target, libname);

			STDOUT.WriteLine(verbalize_deps_path(location));

			copy_dep_pdb_into_build_dir(location);

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
			copy_dep_pdb_into_build_dir(location);
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
			ADD_FLAG("ARFLAGS" + target, '/libpath:"' + p + '" ');
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
		/* Not found in the defaults or the explicit paths,
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
	} else if (!configure_hdr.Exists('HAVE_' + sym)) {
		AC_DEFINE("HAVE_" + sym, have, "have the " + header_name + " header file");
	}

	return p;
}

/* XXX check whether some manifest was originally supplied, otherwise keep using the default. */
function generate_version_info_manifest(makefiletarget)
{
	var manifest_name = makefiletarget + ".manifest";

	if (MODE_PHPIZE) {
		MFO.WriteLine("$(BUILD_DIR)\\" + manifest_name + ": " + PHP_DIR + "\\build\\default.manifest");
		MFO.WriteLine("\t" + CMD_MOD2 + "copy " + PHP_DIR + "\\build\\default.manifest $(BUILD_DIR)\\" + makefiletarget + ".manifest >nul");
	} else {
		MFO.WriteLine("$(BUILD_DIR)\\" + manifest_name + ": win32\\build\\default.manifest");
		MFO.WriteLine("\t" + CMD_MOD2 + "copy $(PHP_SRC_DIR)\\win32\\build\\default.manifest $(BUILD_DIR)\\" + makefiletarget + ".manifest >nul");
	}

	return manifest_name;
}

/* Emits rule to generate version info for a SAPI
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
		MFO.WriteLine("\t" + CMD_MOD1 + "$(RC) /nologo $(BASE_INCLUDES) /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"' +
			makefiletarget + '\\"" /d PRODUCT_NAME="\\"' + res_prod_name +
			versioning + '\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" ' +
			creditspath + '\\template.rc');
		return resname;
	}
	if (MODE_PHPIZE) {
		MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": $(PHP_DIR)\\build\\template.rc");
		MFO.WriteLine("\t" + CMD_MOD1 + "$(RC) /nologo  $(BASE_INCLUDES) /I $(PHP_DIR)/include /n /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"'
			+ makefiletarget + '\\"" /d URL="\\"' + project_url +
			'\\"" /d INTERNAL_NAME="\\"' + internal_name + versioning +
			'\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" $(PHP_DIR)\\build\\template.rc');
	} else {
		MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": win32\\build\\template.rc");
		MFO.WriteLine("\t" + CMD_MOD1 + "$(RC) /nologo  $(BASE_INCLUDES) /n /fo $(BUILD_DIR)\\" + resname + logo + debug +
			' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"'
			+ makefiletarget + '\\"" /d URL="\\"' + project_url +
			'\\"" /d INTERNAL_NAME="\\"' + internal_name + versioning +
			'\\"" /d THANKS_GUYS="\\"' + thanks + '\\"" win32\\build\\template.rc');
	}
	MFO.WriteBlankLines(1);
	return resname;
}

/* Check if PGO is enabled for given module. To disable PGO for a particular module,
define a global variable by the following name scheme before SAPI() or EXTENSION() call
var PHP_MYMODULE_PGO = false; */
function is_pgo_desired(mod)
{
	var varname = "PHP_" + mod.toUpperCase() + "_PGO";

	/* XXX enable PGO in phpize mode */
	if (MODE_PHPIZE) {
		return false;
	}

	/* don't disable if there's no mention of the varname */
	if (eval("typeof " + varname + " == 'undefined'")) {
		return true;
	}

	return eval("!!" + varname);
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

	manifest_name = generate_version_info_manifest(makefiletarget);

	MFO.WriteLine(makefiletarget + ": $(BUILD_DIR)\\" + makefiletarget);
	MFO.WriteLine("\t" + CMD_MOD2 + "echo SAPI " + sapiname_for_printing + " build complete");
	if (MODE_PHPIZE) {
		MFO.WriteLine("$(BUILD_DIR)\\" + makefiletarget + ": $(DEPS_" + SAPI + ") $(" + SAPI + "_GLOBAL_OBJS) $(PHPLIB) $(BUILD_DIR)\\" + resname + " $(BUILD_DIR)\\" + manifest_name);
	} else {
		MFO.WriteLine("$(BUILD_DIR)\\" + makefiletarget + ": $(DEPS_" + SAPI + ") $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname  + " $(BUILD_DIR)\\" + manifest_name);
	}

	var is_lib = makefiletarget.match(new RegExp("\\.lib$"));
	if (makefiletarget.match(new RegExp("\\.dll$"))) {
		ldflags = "/dll $(LDFLAGS)";
		manifest = "-" + CMD_MOD2 + "$(_VC_MANIFEST_EMBED_DLL)";
	} else if (is_lib) {
		ldflags = "$(ARFLAGS)";
		ld = CMD_MOD1 + "$(MAKE_LIB)";
	} else {
		ldflags = "$(LDFLAGS)";
		manifest = "-" + CMD_MOD2 + "$(_VC_MANIFEST_EMBED_EXE)";
	}

	if (PHP_SANITIZER == "yes") {
		if (CLANG_TOOLSET) {
			add_asan_opts("CFLAGS_" + SAPI, "LIBS_" + SAPI, (is_lib ? "ARFLAGS_" : "LDFLAGS_") + SAPI);
		}
	}

	if(is_pgo_desired(sapiname) && (PHP_PGI == "yes" || PHP_PGO != "no")) {
		// Add compiler and link flags if PGO options are selected
		if (PHP_DEBUG != "yes" && PHP_PGI == "yes") {
			ADD_FLAG('CFLAGS_' + SAPI, "/GL /O2");
			ADD_FLAG('LDFLAGS_' + SAPI, "/LTCG /GENPROFILE");
			if (VCVERS >= 1914) {
				ADD_FLAG('LDFLAGS_' + SAPI, "/d2:-FuncCache1");
			}
		}
		else if (PHP_DEBUG != "yes" && PHP_PGO != "no") {
			ADD_FLAG('CFLAGS_' + SAPI, "/GL /O2");
			ADD_FLAG('LDFLAGS_' + SAPI, "/LTCG /USEPROFILE");
			if (VCVERS >= 1914) {
				ADD_FLAG('LDFLAGS_' + SAPI, "/d2:-FuncCache1");
			}
		}

		ldflags += " /PGD:$(PGOPGD_DIR)\\" + makefiletarget.substring(0, makefiletarget.indexOf(".")) + ".pgd";
	}

	if (MODE_PHPIZE) {
		if (ld) {
			MFO.WriteLine("\t" + ld + " /nologo /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(" + SAPI + "_GLOBAL_OBJS_RESP) $(PHPLIB) $(ARFLAGS_" + SAPI + ") $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname);
		} else {
			ld = CMD_MOD1 + '"$(LINK)"';
			MFO.WriteLine("\t" + ld + " /nologo " + " $(" + SAPI + "_GLOBAL_OBJS_RESP) $(PHPLIB) $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname + " /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(LDFLAGS_" + SAPI + ")");
		}
	} else {
		if (ld) {
			MFO.WriteLine("\t" + ld + " /nologo /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(" + SAPI + "_GLOBAL_OBJS_RESP) $(BUILD_DIR)\\$(PHPLIB) $(ARFLAGS_" + SAPI + ") $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname);
		} else {
			ld = CMD_MOD1 + '"$(LINK)"';
			MFO.WriteLine("\t" + ld + " /nologo " + " $(" + SAPI + "_GLOBAL_OBJS_RESP) $(BUILD_DIR)\\$(PHPLIB) $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname + " /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(LDFLAGS_" + SAPI + ")");
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

		ADD_FLAG("LIBS_" + EXT, "php_" + dependson + ".lib");
		if (MODE_PHPIZE) {
			ADD_FLAG("LDFLAGS_" + EXT, "/libpath:$(BUILD_DIR_DEV)\\lib");
			ADD_FLAG("DEPS_" + EXT, "$(BUILD_DIR_DEV)\\lib\\php_" + dependson + ".lib");
		} else {
			ADD_FLAG("LDFLAGS_" + EXT, "/libpath:$(BUILD_DIR)");
			ADD_FLAG("DEPS_" + EXT, "$(BUILD_DIR)\\php_" + dependson + ".lib");
		}

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

var static_pgo_enabled = false;

function ZEND_EXTENSION(extname, file_list, shared, cflags, dllname, obj_dir)
{
	EXTENSION(extname, file_list, shared, cflags, dllname, obj_dir);

	extensions_enabled[extensions_enabled.length - 1][2] = true;
}

function EXTENSION(extname, file_list, shared, cflags, dllname, obj_dir)
{
	var objs = null;
	var EXT = extname.toUpperCase();
	var extname_for_printing;
	var ldflags;

	if (shared == null) {
		if (force_all_shared()) {
			shared = true;
			eval("PHP_" + EXT + "_SHARED = true;");
		} else {
			eval("shared = PHP_" + EXT + "_SHARED;");
		}
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
		var ld = CMD_MOD1 + '"$(LINK)"';
		var manifest_name = generate_version_info_manifest(dllname);

		ldflags = "";
		if (is_pgo_desired(extname) && (PHP_PGI == "yes" || PHP_PGO != "no")) {
			// Add compiler and link flags if PGO options are selected
			if (PHP_DEBUG != "yes" && PHP_PGI == "yes") {
				ADD_FLAG('LDFLAGS_' + EXT, "/LTCG /GENPROFILE");
				if (VCVERS >= 1914) {
					ADD_FLAG('LDFLAGS_' + EXT, "/d2:-FuncCache1");
				}
			}
			else if (PHP_DEBUG != "yes" && PHP_PGO != "no") {
				ADD_FLAG('LDFLAGS_' + EXT, "/LTCG /USEPROFILE");
				if (VCVERS >= 1914) {
					ADD_FLAG('LDFLAGS_' + EXT, "/d2:-FuncCache1");
				}
			}

			ADD_FLAG('CFLAGS_' + EXT, "/GL /O2");

			ldflags = " /PGD:$(PGOPGD_DIR)\\" + dllname.substring(0, dllname.indexOf(".")) + ".pgd";
		}

		MFO.WriteLine("$(BUILD_DIR)\\" + libname + ": $(BUILD_DIR)\\" + dllname);
		MFO.WriteBlankLines(1);
		if (MODE_PHPIZE) {
			MFO.WriteLine("$(BUILD_DIR)\\" + dllname + ": $(DEPS_" + EXT + ") $(" + EXT + "_GLOBAL_OBJS) $(PHPLIB) $(BUILD_DIR)\\" + resname + " $(BUILD_DIR)\\" + manifest_name);
			MFO.WriteLine("\t" + ld + " $(" + EXT + "_GLOBAL_OBJS_RESP) $(PHPLIB) $(LIBS_" + EXT + ") $(LIBS) $(BUILD_DIR)\\" + resname + " /out:$(BUILD_DIR)\\" + dllname + " $(DLL_LDFLAGS) $(LDFLAGS) $(LDFLAGS_" + EXT + ")");
		} else {
			MFO.WriteLine("$(BUILD_DIR)\\" + dllname + ": $(DEPS_" + EXT + ") $(" + EXT + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname + " $(BUILD_DIR)\\" + manifest_name);
			MFO.WriteLine("\t" + ld + " $(" + EXT + "_GLOBAL_OBJS_RESP) $(BUILD_DIR)\\$(PHPLIB) $(LIBS_" + EXT + ") $(LIBS) $(BUILD_DIR)\\" + resname + " /out:$(BUILD_DIR)\\" + dllname + ldflags + " $(DLL_LDFLAGS) $(LDFLAGS) $(LDFLAGS_" + EXT + ")");
		}
		MFO.WriteLine("\t-" + CMD_MOD2 + "$(_VC_MANIFEST_EMBED_DLL)");
		MFO.WriteBlankLines(1);

		if (configure_module_dirname.match("pecl")) {
			ADD_FLAG("PECL_TARGETS", dllname);
		} else {
			ADD_FLAG("EXT_TARGETS", dllname);
		}
		MFO.WriteLine(dllname + ": $(BUILD_DIR)\\" + dllname);
		MFO.WriteLine("\t" + CMD_MOD2 + "echo EXT " + extname + " build complete");
		MFO.WriteBlankLines(1);

		DEFINE('CFLAGS_' + EXT + '_OBJ', '$(CFLAGS_' + EXT + ')');
	} else {
		ADD_FLAG("STATIC_EXT_OBJS", "$(" + EXT + "_GLOBAL_OBJS)");
		ADD_FLAG("STATIC_EXT_OBJS_RESP", "$(" + EXT + "_GLOBAL_OBJS_RESP)");
		ADD_FLAG("STATIC_EXT_LIBS", "$(LIBS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_LDFLAGS", "$(LDFLAGS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_CFLAGS", "$(CFLAGS_" + EXT + ")");
		if (is_pgo_desired(extname) && (PHP_PGI == "yes" || PHP_PGO != "no")) {
			if (!static_pgo_enabled) {
				if (PHP_DEBUG != "yes" && PHP_PGI == "yes") {
					ADD_FLAG('STATIC_EXT_LDFLAGS', "/LTCG:PGINSTRUMENT");
				}
				else if (PHP_DEBUG != "yes" && PHP_PGO != "no") {
					ADD_FLAG('STATIC_EXT_LDFLAGS', "/LTCG:PGUPDATE");
				}

				ADD_FLAG("STATIC_EXT_CFLAGS", "/GL /O2");
				static_pgo_enabled = true;
			}
		}

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
	if (MODE_PHPIZE) {
		if (!FSO.FileExists(PHP_DIR + "/include/main/config.pickle.h")) {
			var _tmp = FSO.CreateTextFile(PHP_DIR + "/include/main/config.pickle.h", true);
			_tmp.Close();
		}
		cflags = "/FI main/config.pickle.h " + cflags;
	}
	ADD_FLAG("CFLAGS_" + EXT, cflags);

	// [extname, shared, zend]
	extensions_enabled[extensions_enabled.length] = [extname, shared ? 'shared' : 'static', false];
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

	var bd = get_define('BUILD_DIR');
	var respd = bd + '\\resp';
	if (!FSO.FolderExists(respd)) {
		FSO.CreateFolder(respd);
	}
	var obj_lst_fn = respd + '\\' + sym + '.txt';
	var resp = "";

	if (configure_subst.Exists(sym)) {
		tv = configure_subst.Item(sym);
	} else {
		if (FSO.FileExists(obj_lst_fn)) {
			FSO.DeleteFile(obj_lst_fn, true);
		}
		tv = "";
	}

	file_list = file_list.split(new RegExp("\\s+"));
	file_list.sort();

	var re = new RegExp("\.[a-z0-9A-Z]+$");

	dir = dir.replace(new RegExp("/", "g"), "\\");
	var objs_line = "";
	var srcs_line = "";

	var sub_build = "$(BUILD_DIR)\\";

	var srcs_by_dir = {};

	/* Parse the file list to create an aggregated structure based on the subdirs passed. */
	for (i in file_list) {
		src = file_list[i];

		var _tmp = src.split("\\");

		var filename = _tmp.pop();

		// build the obj out dir and use it as a key
		var dirname = _tmp.join("\\");

		//WARNING("dir: " + dir + " dirname: " + dirname + " filename: " + filename);

		/* if module dir is not a child of the main source dir,
		 * we need to tweak it; we should have detected such a
		 * case in condense_path and rewritten the path to
		 * be relative.
		 * This probably breaks for non-sibling dirs, but that
		 * is not a problem as buildconf only checks for pecl
		 * as either a child or a sibling */
		if (obj_dir == null) {
			if (MODE_PHPIZE) {
				/* In the phpize mode, the subdirs are always relative to BUID_DIR.
					No need to differentiate by extension, only one gets built. */
				var build_dir = (dirname ? dirname : "").replace(new RegExp("^..\\\\"), "");
			} else {
				var build_dir = (dirname ? (dir + "\\" + dirname) : dir).replace(new RegExp("^..\\\\"), "");
			}
		}
		else {
			var build_dir = (dirname ? obj_dir + "\\" + dirname : obj_dir).replace(new RegExp("^..\\\\"), "");
		}

		obj = sub_build + build_dir + "\\" + filename.replace(re, ".obj");

		if (i > 0) {
			srcs_line += " " + dir + "\\" + src;
			objs_line += " " + obj
		} else {
			srcs_line = dir + "\\" + src;
			objs_line = obj;
		}

		resp += " " + obj.replace('$(BUILD_DIR)', bd);
		tv += " " + obj;

		if (!srcs_by_dir.hasOwnProperty(build_dir)) {
			srcs_by_dir[build_dir] = [];
		}

		/* storing the index from the file_list */
		srcs_by_dir[build_dir].push(i);
	}

	/* Create makefile build targets and dependencies. */
	MFO.WriteLine(objs_line + ": " + srcs_line);

	/* Create target subdirs if any and produce the compiler calls, /mp is respected if enabled. */
	for (var k in srcs_by_dir) {
		var dirs = k.split("\\");
		var i, d = "";
		for (i = 0; i < dirs.length; i++) {
			d += dirs[i];
			build_dirs[build_dirs.length] = d;
			d += "\\";
		}

		var mangle_dir = k.replace(new RegExp("[\\\\/.-]", "g"), "_");
		var bd_flags_name = "CFLAGS_BD_" + mangle_dir.toUpperCase();

		DEFINE(bd_flags_name, "/Fp" + sub_build + d + " /FR" + sub_build + d + " ");
		if (VS_TOOLSET) {
			ADD_FLAG(bd_flags_name, "/Fd" + sub_build + d);
		}

		if (PHP_ANALYZER == "clang") {
			var analyzer_base_args = X64 ? "-m64" : "-m32";
			var analyzer_base_flags = "";

			analyzer_base_args += " --analyze";

			var vc_ver;
			if (VS_TOOLSET) {
				vc_ver = VCVERS;
			} else {
				vc_ver = probe_binary(PATH_PROG('cl', null));
			}

			analyzer_base_args += " -fms-compatibility -fms-compatibility-version=" + vc_ver + " -fms-extensions -Xclang -analyzer-output=text -Xclang -fmodules";
		} else if (PHP_ANALYZER == "cppcheck") {
			var analyzer_base_args = "";
			var analyzer_base_flags = "";

			if (VS_TOOLSET) {
				analyzer_base_flags += " -D _MSC_VER=" + VCVERS;
			} else {
				analyzer_base_flags += " -D _MSC_VER=" + probe_binary(PATH_PROG('cl', null));
			}

			if (X64) {
				analyzer_base_flags += " -D _M_X64 -D _WIN64";
			} else {
				analyzer_base_flags += " -D _M_IX86 ";
			}
			analyzer_base_flags += " -D _WIN32 -D WIN32 -D _WINDOWS";

			var vc_incs = WshShell.Environment("Process").Item("INCLUDE").split(";")
			for (i in vc_incs) {
				if (!vc_incs[i].length) {
					continue;
				}
				analyzer_base_flags += " -I " + "\"" + vc_incs[i] + "\"";
			}

			var cppcheck_platform = X64 ? "win64" : "win32A";
			var cppcheck_lib = "win32\\build\\cppcheck_" + (X64 ? "x64" : "x86") + ".cfg";
			analyzer_base_args += "--enable=warning,performance,portability,information,missingInclude " +
						"--platform=" + cppcheck_platform + " " +
						"--library=windows.cfg --library=microsoft_sal.cfg " +
						"--library=win32\\build\\cppcheck.cfg " +
						"--library=" + cppcheck_lib + " " +
						/* "--rule-file=win32\build\cppcheck_rules.xml " + */
						" --std=c89 --std=c++11 " +
						"--quiet --inconclusive --template=vs -j 4 " +
						"--suppress=unmatchedSuppression " +
						"--suppressions-list=win32\\build\\cppcheck_suppress.txt ";

			var cppcheck_build_dir = get_define("CPPCHECK_BUILD_DIR");
			if (!!cppcheck_build_dir) {
				analyzer_base_args += "--cppcheck-build-dir=$(CPPCHECK_BUILD_DIR)";
			}
		}

		if (PHP_MP_DISABLED) {
			for (var j in srcs_by_dir[k]) {
				src = file_list[srcs_by_dir[k][j]];

				var _tmp = src.split("\\");
				var filename = _tmp.pop();
				obj = filename.replace(re, ".obj");

				MFO.WriteLine("\t" + CMD_MOD1 + "$(CC) $(" + flags + ") $(CFLAGS) $(" + bd_flags_name + ") /c " + dir + "\\" + src + " /Fo" + sub_build + d + obj);

				if ("clang" == PHP_ANALYZER) {
					MFO.WriteLine("\t" + CMD_MOD1 + "\"$(CLANG_CL)\" " + analyzer_base_args + " $(" + flags + "_ANALYZER) $(CFLAGS_ANALYZER) $(" + bd_flags_name + "_ANALYZER) " + dir + "\\" + src);
				} else if ("cppcheck" == PHP_ANALYZER) {
					MFO.WriteLine("\t\"" + CMD_MOD1 + "$(CPPCHECK)\" " + analyzer_base_args + " $(" + flags + "_ANALYZER) $(CFLAGS_ANALYZER) $(" + bd_flags_name + "_ANALYZER) " + analyzer_base_flags + " " + dir + "\\" + src);
				}else if (PHP_ANALYZER == "pvs") {
					MFO.WriteLine("\t" + CMD_MOD1 + "\"$(PVS_STUDIO)\" --cl-params $(" + flags + ") $(CFLAGS) $(" + bd_flags_name + ") /c " + dir + "\\" + src + " --source-file "  + dir + "\\" + src
						+ " --cfg PVS-Studio.conf --errors-off \"V122 V117 V111\" ");
				}
			}
		} else {
			/* TODO create a response file at least for the source files to work around the cmd line length limit. */
			var src_line = "";
			for (var j in srcs_by_dir[k]) {
				src_line += dir + "\\" + file_list[srcs_by_dir[k][j]] + " ";
			}

			MFO.WriteLine("\t" + CMD_MOD1 + "$(CC) $(" + flags + ") $(CFLAGS) /Fo" + sub_build + d + " $(" + bd_flags_name + ") /c " + src_line);

			if ("clang" == PHP_ANALYZER) {
				MFO.WriteLine("\t\"$(CLANG_CL)\" " + analyzer_base_args + " $(" + flags + "_ANALYZER) $(CFLAGS_ANALYZER)  $(" + bd_flags_name + "_ANALYZER) " + src_line);
			} else if ("cppcheck" == PHP_ANALYZER) {
				MFO.WriteLine("\t\"$(CPPCHECK)\" " + analyzer_base_args + " $(" + flags + "_ANALYZER) $(CFLAGS_ANALYZER)  $(" + bd_flags_name + "_ANALYZER) " + analyzer_base_flags + " " + src_line);
			}
		}
	}

	DEFINE(sym, tv);

	/* Generate the object response file and define it to the Makefile. This can be
	   useful when getting the "command line too long" linker errors.
	   TODO pack this into a function when response files are used for other kinds of info. */
	var obj_lst_fh = null;
	if (!FSO.FileExists(obj_lst_fn)) {
		obj_lst_fh = FSO.CreateTextFile(obj_lst_fn);
	} else {
		obj_lst_fh = FSO.OpenTextFile(obj_lst_fn, 8);
	}

	obj_lst_fh.Write(" " + resp);
	obj_lst_fh.Close();
	DEFINE(sym + "_RESP", '@"' + obj_lst_fn + '"');
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
	var fixedlength = "";
	var t = 0;
	var i,j,k,m;
	var out = "| ";
	var min = new Array(l);
	var max = new Array(l);

	if (!!ar_out[0] && l != ar_out[0].length) {
		STDOUT.WriteLine("Invalid header argument, can't output the table " + l + " " + ar_out[0].length  );
		return;
	}

	for (j=0; j < l; j++) {
		var tmax, tmin;

		/* Figure out the max length per column */
		tmin = 0;
		tmax = 0;
		for (k = 0; k < ar_out.length; k++) {
			if(typeof ar_out[k][j] != 'undefined') {
				var t = ar_out[k][j].length;
				if (t > tmax) tmax = t;
				else if (t < tmin) tmin = t;
			}
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
			if(typeof line[j] != 'undefined') {
				for (var k = 0; k < (max[j] - line[j].length); k++){
					out += " ";
				}
			}
			out += " |";
		}
		STDOUT.WriteLine(out);
		out = "|";
	}

	STDOUT.WriteLine(sep);
}

function write_extensions_summary()
{
	var exts = new Array();
	var zend_exts = new Array();

	for(var x = 0; x < extensions_enabled.length; ++x)
	{
		var l = extensions_enabled[x];

		if(l[2])
		{
			zend_exts.push([l[0], l[1]]);
		}
		else
		{
			exts.push([l[0], l[1]]);
		}
	}

	STDOUT.WriteLine('Enabled extensions:');
	output_as_table(['Extension', 'Mode'], exts.sort());

	if(zend_exts.length)
	{
		STDOUT.WriteBlankLines(2);
		STDOUT.WriteLine('Enabled Zend extensions:');
		output_as_table(['Extension', 'Mode'], zend_exts.sort());
	}
}

function write_summary()
{
	var ar = new Array();
	var k = 0;

	STDOUT.WriteBlankLines(2);
	write_extensions_summary();
	STDOUT.WriteBlankLines(2);
	if (!MODE_PHPIZE) {
		STDOUT.WriteLine("Enabled SAPI:");
		output_as_table(["Sapi Name"], sapi_enabled);
		STDOUT.WriteBlankLines(2);
	}
	ar[k++] = ['Build type', PHP_DEBUG == "yes" ? "Debug" : "Release"];
	ar[k++] = ['Thread Safety', PHP_ZTS == "yes" ? "Yes" : "No"];
	ar[k++] = ['Compiler', COMPILER_NAME_LONG];
	ar[k++] = ['Architecture', X64 ? 'x64' : 'x86'];
	if (PHP_PGO == "yes") {
		ar[k++] = ['Optimization', "PGO"];
	} else if (PHP_PGI == "yes") {
		ar[k++] = ['Optimization', "PGI"];
	} else {
		ar[k++] = ['Optimization', PHP_DEBUG == "yes" ? "disabled" : "PGO disabled"];
	}
	var simd = configure_subst.Item("PHP_SIMD_SCALE");
	if (!!simd) {
		ar[k++] = ["Native intrinsics", simd];
	}
	if (PHP_ANALYZER == "vs") {
		ar[k++] = ['Static analyzer', 'Visual Studio'];
	} else if (PHP_ANALYZER == "clang") {
		ar[k++] = ['Static analyzer', 'clang'];
	} else if (PHP_ANALYZER == "cppcheck") {
		ar[k++] = ['Static analyzer', 'Cppcheck'];
	} else if (PHP_ANALYZER == "pvs") {
		ar[k++] = ['Static analyzer', 'PVS-Studio'];
	} else {
		ar[k++] = ['Static analyzer', 'disabled'];
	}

	output_as_table(["",""], ar);
	STDOUT.WriteBlankLines(2);
}

function is_on_exclude_list_for_test_ini(list, name)
{
	for (var i in list) {
		if (name == list[i]) {
			return true;
		}
	}

	return false;
}

function generate_tmp_php_ini()
{
	if ("no" == PHP_TEST_INI) {
		/* Test ini generation is disabled. */
		return;
	}

	var ini_dir = PHP_OBJECT_OUT_DIR + ("yes" == PHP_DEBUG ? "Debug" : "Release") + ("yes" == PHP_ZTS ? "_TS" : "");
	PHP_TEST_INI_PATH = ini_dir + "\\tmp-php.ini";

	if (FSO.FileExists(PHP_TEST_INI_PATH)) {
		STDOUT.WriteLine("Generating " + PHP_TEST_INI_PATH + " ...");
		var INI = FSO.OpenTextFile(PHP_TEST_INI_PATH, 2);
	} else {
		STDOUT.WriteLine("Regenerating " + PHP_TEST_INI_PATH + " ...");
		var INI = FSO.CreateTextFile(PHP_TEST_INI_PATH, true);
	}

	var ext_list = PHP_TEST_INI_EXT_EXCLUDE.split(",");
	INI.WriteLine("extension_dir=" + ini_dir);
	for (var i in extensions_enabled) {
		if ("shared" != extensions_enabled[i][1]) {
			continue;
		}

		var directive = (extensions_enabled[i][2] ? 'zend_extension' : 'extension');

		var ext_name = extensions_enabled[i][0];
		if ("gd" == ext_name) {
			ext_name = "gd2";
		}

		if (!is_on_exclude_list_for_test_ini(ext_list, ext_name)) {
			INI.WriteLine(directive + "=php_" + ext_name + ".dll");

			if ("opcache" == ext_name) {
				var dir = get_define("BUILD_DIR") + "\\" + "test_file_cache";
				if (FSO.FolderExists(dir)) {
					STDOUT.Write(execute("powershell -Command Remove-Item -path \"\\\\?\\" + dir + "\" -recurse"));
				}
				FSO.CreateFolder(dir);

				/* Fallback is implied, if filecache is enabled. */
				INI.WriteLine("opcache.file_cache=" + dir);
				INI.WriteLine("opcache.enable=1");
				INI.WriteLine("opcache.enable_cli=1");
			}
		}
	}

	INI.Close();
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

	STDOUT.WriteLine("Generating files...");
	if (!MODE_PHPIZE) {
		generate_tmp_php_ini();
	}
	generate_makefile();
	if (!MODE_PHPIZE) {
		generate_internal_functions();
		generate_config_h();
		generate_phpize();
	} else {
		generate_config_pickle_h();
		generate_ext_pickle();
	}
	STDOUT.WriteLine("Done.");
	STDOUT.WriteBlankLines(1);
	write_summary();

	if (INVALID_CONFIG_ARGS.length) {
		STDOUT.WriteLine('WARNING');
		STDOUT.WriteLine('The following arguments is invalid, and therefore ignored:');

		for (var i = 0; i < INVALID_CONFIG_ARGS.length; ++i) {
			STDOUT.WriteLine(' ' + INVALID_CONFIG_ARGS[i]);
		}

		STDOUT.WriteBlankLines(2);
	}

	if (PHP_SNAPSHOT_BUILD != "no") {
		STDOUT.WriteLine("Type 'nmake snap' to build a PHP snapshot");
	} else {
		STDOUT.WriteLine("Type 'nmake' to build PHP");
	}
}

function generate_ext_pickle()
{
	var content;
	var DEPS = null;
	var dest;
	var deps_lines = new Array();

	var build_var_name = function(name) {
		return "PHP_" + name.toUpperCase();
	}

	STDOUT.WriteLine("Generating pickle deps");
	dest = PHP_DIR + "/script/";

	if (!FSO.FolderExists(dest)) {
		FSO.CreateFolder(dest);
	}

	if (FSO.FileExists(dest + "/ext_pickle.js")) {
		DEPS = FSO.OpenTextFile(dest + "/ext_pickle.js", 1);

		while (!DEPS.AtEndOfStream) {
			var ln = DEPS.ReadLine();
			var found = false;

			for (var i in extensions_enabled) {
				var reg0 = new RegExp(build_var_name(extensions_enabled[i][0]) + "\s*=.+", "g");
				var reg1 = new RegExp(build_var_name(extensions_enabled[i][0]) + "_SHARED" + "\s*=.+", "g");

				if (ln.match(reg1) || ln.match(reg0)) {
					found = true;
					break;
				}
			}

			if (!found) {
				deps_lines.push(ln);
			}
		}
	}

	for (var i in extensions_enabled) {
		deps_lines.push(build_var_name(extensions_enabled[i][0]) + "=true;");
		deps_lines.push(build_var_name(extensions_enabled[i][0]) + "_SHARED=" + (extensions_enabled[i][1] == 'shared' ? 'true' : 'false') + ";");
	}

	if (!!DEPS) {
		DEPS.Close();
		DEPS = null;
	}

	/* Replace the ext_pickle.js with the new content */
	DEPS = FSO.CreateTextFile(dest + "/ext_pickle.js", true);

	for (var j in deps_lines) {
		DEPS.WriteLine(deps_lines[j]);
	}

	DEPS.Close();
}

function generate_config_pickle_h()
{
	var outfile = null;
	var lines = new Array();
	var keys = (new VBArray(configure_hdr.Keys())).toArray();
	dest = PHP_DIR + "/include/main";

	var ignore_key = function(key) {
		var ignores = [ "CONFIGURE_COMMAND", "PHP_COMPILER_ID", "COMPILER", "ARCHITECTURE", "HAVE_STRNLEN", "PHP_DIR" ];

		for (var k in ignores) {
			if (ignores[k] == key) {
				return true;
			}
		}

		return false;
	}


	STDOUT.WriteLine("Generating main/config.pickle.h");

	if (FSO.FileExists(dest + "/config.pickle.h")) {
		outfile = FSO.OpenTextFile(dest + "/config.pickle.h", 1);

		while (!outfile.AtEndOfStream) {
			var found = false;
			var ln = outfile.ReadLine();

			for (var i in keys) {
				var reg = new RegExp("#define[\s ]+" + keys[i] + "[\s ]*.*", "g");

				if (ln.match(reg)) {
					found = true;
					break;
				}
			}

			if (!found) {
				lines.push(ln);
			}
		}
	}

	for (var i in keys) {
		var item = configure_hdr.Item(keys[i]);

		if (ignore_key(keys[i])) {
			continue;
		}

		/* XXX fix comment handling */
		/*if (!lines[j].match(/^#define.+/g)) {
			continue;
		}*/

		lines.push("#define " + keys[i] + " " + item[0]);
	}

	if (outfile) {
		outfile.Close();
		outfile = null;
	}

	outfile = FSO.CreateTextFile(dest + "/config.pickle.h", true);

	for (var k in lines) {
		outfile.WriteLine(lines[k]);
	}

	outfile.Close();
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

	if (VS_TOOLSET) {
		if (VCVERS >= 1800) {
			outfile.WriteLine("");
			outfile.WriteLine("#define HAVE_ACOSH 1");
			outfile.WriteLine("#define HAVE_ASINH 1");
			outfile.WriteLine("#define HAVE_ATANH 1");
		}
		if (VCVERS >= 1900) {
			outfile.WriteLine("#define HAVE_LOG1P 1");
		}
	}


	outfile.Close();
}

/* Generate phpize */
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
	MF.WriteLine("var PHP_EXTRA_VERSION=\"" + PHP_EXTRA_VERSION + "\"");
	MF.WriteLine("var PHP_VERSION_STRING=\"" + PHP_VERSION_STRING + "\"");
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
	CJ.WriteLine("var PHP_DEBUG=" + '"' + PHP_DEBUG + '"');
	CJ.WriteLine("var PHP_DLL_LIB =" + '"' + get_define('PHPLIB') + '"');
	CJ.WriteLine("var PHP_DLL =" + '"' + get_define('PHPDLL') + '"');
	CJ.WriteLine("var PHP_SECURITY_FLAGS =" + '"' + PHP_SECURITY_FLAGS + '"');

	/* The corresponding configure options aren't enabled through phpize,
		thus these dummy declarations are required. */
	CJ.WriteLine("var PHP_ANALYZER =" + '"no"');
	CJ.WriteLine("var PHP_PGO =" + '"no"');
	CJ.WriteLine("var PHP_PGI =" + '"no"');
	CJ.WriteLine("var PHP_ALL_SHARED =" + '"no"');

	CJ.WriteBlankLines(1);
	CJ.Close();
}

function extract_convert_style_line(val, match_sw, to_sw, keep_mkfile_vars)
{
	var ret = "";

	/*var re = new RegExp(match_sw + "(.*)", "g");
	var r;

	while (r = re.execute(val)) {
		WARNING(r);
	}
	return ret;*/

	var cf = val.replace(/\s+/g, " ").split(" ");

	var i_val = false;
	for (var i in cf) {
		var r;

		if (keep_mkfile_vars) {
			r = cf[i].match(/^\$\((.*)\)/);
			if (!!r) {
				ret += " " + r[0];
				continue;
			}
		}

		if (i_val && !!cf[i]) {
			i_val = false;
			ret += " " + to_sw + " " + cf[i];
			continue;
		}

		var re;

		re = new RegExp(match_sw + "(.*)");
		r = cf[i].match(re);
		if (!!r && r.length > 1 && !!r[1]) {
			/* The value is not ws separated from the switch. */
			ret += " " + to_sw + " " + r[1];
			continue;
		}

		r = cf[i].match(match_sw);
		if (!!r) {
			//WARNING(cf[i]);
			/* Value is going to be added in the next iteration. */
			i_val = true;
		}
	}

	return ret;
}

function handle_analyzer_makefile_flags(fd, key, val)
{
	var relevant = false;

	/* VS integrates /analyze with the bulid process,
		no further action is required. */
	if ("no" == PHP_ANALYZER || "vs" == PHP_ANALYZER) {
		return;
	}

	if (key.match("CFLAGS")) {
		var new_val = val;
		var reg = /\$\(([^\)]+)\)/g;
		var r;
		while (r = reg.exec(val)) {
			var repl = "$(" + r[1] + "_ANALYZER)"
			new_val = new_val.replace(r[0], repl);
		}
		val = new_val;

		if ("clang" == PHP_ANALYZER) {
			val = val.replace(/\/FD /, "")
				.replace(/\/Fp.+? /, "")
				.replace(/\/Fo.+? /, "")
				.replace(/\/Fd.+? /, "")
				//.replace(/\/Fd.+?/, " ")
				.replace(/\/FR.+? /, "")
				.replace("/guard:cf ", "")
				.replace(/\/MP \d+ /, "")
				.replace(/\/MP /, "")
				.replace("/LD ", "")
				.replace("/Qspectre ", "");
		} else if ("cppcheck" == PHP_ANALYZER) {
			new_val = "";

			new_val += extract_convert_style_line(val, "/I", "-I", true);
			new_val += extract_convert_style_line(val, "/D", "-D", false);

			val = new_val;
		}

		relevant = true;
	} else if (key.match("BASE_INCLUDES")) {
		if ("cppcheck" == PHP_ANALYZER) {
			new_val = "";

			new_val += extract_convert_style_line(val, "/I", "-I", true);
			new_val += extract_convert_style_line(val, "/D", "-D", false);

			val = new_val;
		}

		relevant = true;
	}

	if (!relevant) {
		return;
	}

	key += "_ANALYZER";
	//WARNING("KEY: " + key + " VAL: " + val);

	fd.WriteLine(key + "=" + val + " ");
	fd.WriteBlankLines(1);
}

/* Generate the Makefile */
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

		/* \s+\/ eliminates extra whitespace caused when using \ for string continuation,
			whereby \/ is the start of the next compiler switch */
		var val = trim(configure_subst.Item(keys[i])).replace(/\s+\//gm, " /");

		MF.WriteLine(keys[i] + "=" + val + " ");
		MF.WriteBlankLines(1);

		/* If static analyze is enabled, add analyzer specific stuff to the Makefile. */
		handle_analyzer_makefile_flags(MF, keys[i], val);
	}

	if (!MODE_PHPIZE) {
		var val = "yes" == PHP_TEST_INI ? PHP_TEST_INI_PATH : "";
		/* Be sure it's done after generate_tmp_php_ini(). */
		MF.WriteLine("PHP_TEST_INI_PATH=\"" + val + "\"");
	}

	MF.WriteBlankLines(1);
	if (MODE_PHPIZE) {
		var TF = FSO.OpenTextFile(PHP_DIR + "/script/Makefile.phpize", 1);
	} else {
		var TF = FSO.OpenTextFile("win32/build/Makefile", 1);
	}

	MF.Write(TF.ReadAll());

	MF.WriteLine("build-headers:");
	MF.WriteLine("	" + CMD_MOD2 + "if not exist $(BUILD_DIR_DEV)\\include mkdir $(BUILD_DIR_DEV)\\include >nul");
	MF.WriteLine("	" + CMD_MOD2 + "for %D in ($(INSTALL_HEADERS_DIR)) do " + CMD_MOD2 + "if not exist $(BUILD_DIR_DEV)\\include\\%D mkdir $(BUILD_DIR_DEV)\\include\\%D >nul");
	for (i in headers_install) {
		if (headers_install[i][2] != "") {
				MF.WriteLine("	" + CMD_MOD2 + "if not exist $(BUILD_DIR_DEV)\\include\\" + headers_install[i][2] + " mkdir $(BUILD_DIR_DEV)\\include\\" +
												headers_install[i][2] + ">nul");
				MF.WriteLine("	" + CMD_MOD2 + "copy " + headers_install[i][0] + " " + "$(BUILD_DIR_DEV)\\include\\" + headers_install[i][2] + " /y >nul");
		}
	}
	MF.WriteLine("	" + CMD_MOD2 + "for %D in ($(INSTALL_HEADERS_DIR)) do " + CMD_MOD2 + "copy %D*.h $(BUILD_DIR_DEV)\\include\\%D /y >nul");
	if (MODE_PHPIZE) {
		MF.WriteBlankLines(1);
		MF.WriteLine("build-bins:");
		for (var i in extensions_enabled) {
			var lib = "php_" + extensions_enabled[i][0] + ".lib";
			var dll = "php_" + extensions_enabled[i][0] + ".dll";
			MF.WriteLine("	" + CMD_MOD2 + "copy $(BUILD_DIR)\\" + lib + " $(BUILD_DIR_DEV)\\lib");
			MF.WriteLine("  " + CMD_MOD2 + "if not exist $(PHP_PREFIX) mkdir $(PHP_PREFIX) >nul");
			MF.WriteLine("  " + CMD_MOD2 + "if not exist $(PHP_PREFIX)\\ext mkdir $(PHP_PREFIX)\\ext >nul");
			MF.WriteLine("	" + CMD_MOD2 + "copy $(BUILD_DIR)\\" + dll + " $(PHP_PREFIX)\\ext");
		}
	} else {
		MF.WriteBlankLines(1);
		MF.WriteLine("build-ext-libs:");
		MF.WriteLine("	" + CMD_MOD2 + "if not exist $(BUILD_DIR_DEV)\\lib mkdir $(BUILD_DIR_DEV)\\lib >nul");
		for (var i in extensions_enabled) {
			var lib;

			lib = "php_" + extensions_enabled[i][0] + "*.lib";

			if ('shared' == extensions_enabled[i][1]) {
				MF.WriteLine("	" + CMD_MOD2 + "if exist $(BUILD_DIR)\\" + lib + " copy $(BUILD_DIR)\\" + lib + " $(BUILD_DIR_DEV)\\lib");
			}
		}
	}
	TF.Close();

	MF.WriteBlankLines(1);

	var extra_path = "$(PHP_BUILD)\\bin";
	if (PHP_EXTRA_LIBS.length) {
		path = PHP_EXTRA_LIBS.split(';');
		for (i = 0; i < path.length; i++) {
			f = FSO.GetAbsolutePathName(path[i] + "\\..\\bin");
			if (FSO.FolderExists(f)) {
				extra_path = extra_path + ";" + f;
			}
		}
	}
	if (PHP_SANITIZER == "yes") {
		if (CLANG_TOOLSET) {
			extra_path = extra_path + ";" + get_clang_lib_dir() + "\\windows";
		}
	}
	MF.WriteLine("set-tmp-env:");
	MF.WriteLine("	" + CMD_MOD2 + "set PATH=" + extra_path + ";$(PATH)");

	MF.WriteBlankLines(2);

	MF.WriteLine("dump-tmp-env: set-tmp-env");
	MF.WriteLine("	" + CMD_MOD2 + "set");

	MF.WriteBlankLines(2);

	MFO.Close();
	TF = FSO.OpenTextFile("Makefile.objects", 1);
	if (!TF.AtEndOfStream) {
		MF.Write(TF.ReadAll());
	}
	TF.Close();
	MF.WriteBlankLines(2);

	if (FSO.FileExists(PHP_MAKEFILE_FRAGMENTS)) {
		TF = FSO.OpenTextFile(PHP_MAKEFILE_FRAGMENTS, 1);
		if (!TF.AtEndOfStream) {
			MF.Write(TF.ReadAll());
		}
		TF.Close();
		MF.WriteBlankLines(2);
		FSO.DeleteFile(PHP_MAKEFILE_FRAGMENTS, true);
	}

	MF.Close();
}

function ADD_FLAG(name, flags, target)
{
	if (target != null) {
		name = target.toUpperCase() + "_" + name;
	}
	flags = trim(flags);
	if (configure_subst.Exists(name)) {
		var curr_flags = configure_subst.Item(name);

		/* Prefix with a space, thus making sure the
		   current flag is not a substring of some
		   other. It's still not a complete check if
		   some flags with spaces got added.

		   TODO rework to use an array, so direct
		        match can be done. This will also
			help to normalize flags and to not
			to insert duplicates. */
		if (curr_flags.indexOf(" " + flags) >= 0 || curr_flags.indexOf(flags + " ") >= 0) {
			return;
		}

		flags = curr_flags + " " + flags;
		configure_subst.Remove(name);
	}
	configure_subst.Add(name, flags);
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
	} else if (typeof(value) != "undefined" && value.length == 0) {
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

/* Install Headers */
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

// For snapshot builders, this option will attempt to enable everything
// and you can then build everything, ignoring fatal errors within a module
// by running "nmake snap"
PHP_SNAPSHOT_BUILD = "no";
if (!MODE_PHPIZE) {
	ARG_ENABLE('snapshot-build', 'Build a snapshot; turns on everything it can and ignores build errors', 'no');
}

function toolset_option_handle()
{
	if ("clang" == PHP_TOOLSET) {
		VS_TOOLSET = false;
		CLANG_TOOLSET = true;
		ICC_TOOLSET = false;
	} else if ("icc" == PHP_TOOLSET) {
		VS_TOOLSET = false;
		CLANG_TOOLSET = false;
		ICC_TOOLSET = true;
	} else {
		/* Visual Studio is the default toolset. */
		PHP_TOOLSET = "no" == PHP_TOOLSET ? "vs" : PHP_TOOLSET;
		if (!!PHP_TOOLSET && "vs" != PHP_TOOLSET) {
			ERROR("Unsupported toolset '" + PHP_TOOLSET + "'");
		}
		VS_TOOLSET = true;
		CLANG_TOOLSET = false;
		ICC_TOOLSET = false;
	}
}

function toolset_setup_compiler()
{
	PHP_CL = toolset_get_compiler();
	if (!PHP_CL) {
		ERROR("Compiler not found");
	}

	COMPILER_NUMERIC_VERSION = toolset_get_compiler_version();
	COMPILER_NAME_LONG = toolset_get_compiler_name();
	COMPILER_NAME_SHORT = toolset_get_compiler_name(true);

	if (VS_TOOLSET) {
		VCVERS = COMPILER_NUMERIC_VERSION;

		if ("unknown" == COMPILER_NAME_LONG) {
			var tmp = probe_binary(PHP_CL);
			COMPILER_NAME_LONG = COMPILER_NAME_SHORT = "MSVC " + tmp + ", untested";

			WARNING("Using unknown MSVC version " + tmp);

			AC_DEFINE('COMPILER', COMPILER_NAME_LONG, "Detected compiler version");
			DEFINE("PHP_COMPILER_SHORT", tmp);
			AC_DEFINE('PHP_COMPILER_ID', tmp, "Compiler compatibility ID");
		} else {
			AC_DEFINE('COMPILER', COMPILER_NAME_LONG, "Detected compiler version");
			DEFINE("PHP_COMPILER_SHORT", COMPILER_NAME_SHORT.toLowerCase());
			AC_DEFINE('PHP_COMPILER_ID', COMPILER_NAME_SHORT.toUpperCase(), "Compiler compatibility ID");
		}
	} else if (CLANG_TOOLSET) {
		CLANGVERS = COMPILER_NUMERIC_VERSION;

		AC_DEFINE('COMPILER', COMPILER_NAME_LONG, "Detected compiler version");
		DEFINE("PHP_COMPILER_SHORT", "clang");
		AC_DEFINE('PHP_COMPILER_ID', "clang"); /* XXX something better were to write here */

	} else if (ICC_TOOLSET) {
		INTELVERS = COMPILER_NUMERIC_VERSION;

		AC_DEFINE('COMPILER', COMPILER_NAME_LONG, "Detected compiler version");
		DEFINE("PHP_COMPILER_SHORT", "icc");
		AC_DEFINE('PHP_COMPILER_ID', "icc"); /* XXX something better were to write here */
	}
	STDOUT.WriteLine("  Detected compiler " + COMPILER_NAME_LONG);
}

function toolset_setup_project_tools()
{
	PATH_PROG('nmake');

	// we don't want to define LIB, as that will override the default library path
	// that is set in that env var
	PATH_PROG('lib', null, 'MAKE_LIB');

	var BISON = PATH_PROG('bison');
	if (BISON) {
		var BISONVERS = probe_binary(BISON, "longversion");
		STDOUT.WriteLine('  Detected bison version ' + BISONVERS);

		if (BISONVERS.match(/^\d+.\d+$/)) {
			BISONVERS += ".0";
		}

		var hm = BISONVERS.match(/(\d+)\.(\d+)\.(\d+)/);
		var nm = MINBISON.match(/(\d+)\.(\d+)\.(\d+)/);

		var intvers =  (hm[1]-0)*10000 + (hm[2]-0)*100 + (hm[3]-0);
		var intmin =  (nm[1]-0)*10000 + (nm[2]-0)*100 + (nm[3]-0);

		if (intvers < intmin) {
			ERROR('The minimum bison version requirement is ' + MINBISON);
		}
	} else {
		ERROR('bison is required')
	}

	if (!PATH_PROG('sed')) {
		ERROR('sed is required')
	}

	var RE2C = PATH_PROG('re2c');
	if (RE2C) {
		var RE2CVERS = probe_binary(RE2C, "version");
		STDOUT.WriteLine('  Detected re2c version ' + RE2CVERS);

		if (RE2CVERS.match(/^\d+.\d+$/)) {
			RE2CVERS += ".0";
		}

		var hm = RE2CVERS.match(/(\d+)\.(\d+)\.(\d+)/);
		var nm = MINRE2C.match(/(\d+)\.(\d+)\.(\d+)/);

		var intvers =  (hm[1]-0)*10000 + (hm[2]-0)*100 + (hm[3]-0);
		var intmin =  (nm[1]-0)*10000 + (nm[2]-0)*100 + (nm[3]-0);

		if (intvers < intmin) {
			ERROR('The minimum RE2C version requirement is ' + MINRE2C);
		}
	} else {
		ERROR('re2c is required')
	}
	PATH_PROG('zip');
	PATH_PROG('lemon');
	PATH_PROG('7za');

	// avoid picking up midnight commander from cygwin
	if (!PATH_PROG('mc', WshShell.Environment("Process").Item("PATH"))) {
		ERROR('mc is required')
	}

	// Try locating the manifest tool
	if (VS_TOOLSET) {
		if (!PATH_PROG('mt', WshShell.Environment("Process").Item("PATH"))) {
			ERROR('mt is required')
		}
	}
}
/* Get compiler if the toolset is supported */
function toolset_get_compiler()
{
	if (VS_TOOLSET) {
		return PATH_PROG('cl', null, 'PHP_CL')
	} else if (CLANG_TOOLSET) {
		return PATH_PROG('clang-cl', null, 'PHP_CL')
	} else if (ICC_TOOLSET) {
		return PATH_PROG('icl', null, 'PHP_CL')
	}

	ERROR("Unsupported toolset");
}

/* Get compiler version if the toolset is supported */
function toolset_get_compiler_version()
{
	var version;

	if (VS_TOOLSET) {
		version = probe_binary(PHP_CL).substr(0, 5).replace('.', '');

		return version;
	} else if (CLANG_TOOLSET) {
		var command = 'cmd /c ""' + PHP_CL + '" -v"';
		var full = execute(command + '" 2>&1"');

		if (full.match(/clang version ([\d\.]+) \((.*)\)/)) {
			version = RegExp.$1;
			version = version.replace(/\./g, '');
			version = version/100 < 1 ? version*10 : version;

			return version;
		}
	} else if (ICC_TOOLSET) {
		var command = 'cmd /c ""' + PHP_CL + '" -v"';
		var full = execute(command + '" 2>&1"');

		if (full.match(/Version (\d+\.\d+\.\d+)/)) {
			version = RegExp.$1;
			version = version.replace(/\./g, '');
			version = version/100 < 1 ? version*10 : version;

			return version;
		}
	}

	ERROR("Failed to parse compiler version or unsupported toolset");
}

/* Get compiler name if the toolset is supported */
function toolset_get_compiler_name(short)
{
	var version;
	short = !!short;

	if (VS_TOOLSET) {
		var name = "unknown";

		version = probe_binary(PHP_CL).substr(0, 5).replace('.', '');

		if (version >= 1930) {
			return name;
		} if (version >= 1920) {
			/* NOTE - VS is intentional. Due to changes in recent Visual Studio
						versioning scheme refering to the exact VC++ version is
						hardly predictable. From this version on, it refers to 
						Visual Studio version and implies the default toolset.
						When new versions are introduced, adapt also checks in
						php_win32_image_compatible(), if needed. */
			name = short ? "VS16" : "Visual C++ 2019";
		} else if (version >= 1910) {
			name = short ? "VC15" : "Visual C++ 2017";
		} else if (version >= 1900) {
			name = short ? "VC14" : "Visual C++ 2015";
		} else {
			ERROR("Unsupported Visual C++ compiler " + version);
		}

		return name;
	} else if (CLANG_TOOLSET || ICC_TOOLSET) {
		var command = 'cmd /c ""' + PHP_CL + '" -v"';
		var full = execute(command + '" 2>&1"');

		return full.split(/\n/)[0].replace(/\s/g, ' ');
	}

	WARNING("Unsupported toolset");
}


function toolset_is_64()
{
	if (VS_TOOLSET) {
		return probe_binary(PHP_CL, 64);
	} else if (CLANG_TOOLSET) {
		/*var command = 'cmd /c ""' + PHP_CL + '" -v"';
		var full = execute(command + '" 2>&1"');

		return null != full.match(/x86_64/);*/

		/* Even executed within an environment setup with vcvars32.bat,
		clang-cl doesn't recognize the arch toolset. But as it needs
		the VS environment, checking the arch of cl.exe is correct. */
		return probe_binary(PATH_PROG('cl', null), 64);
	} else if (ICC_TOOLSET) {
		var command = 'cmd /c ""' + PHP_CL + '" -v"';
		var full = execute(command + '" 2>&1"');

		return null != full.match(/Intel\(R\) 64/);
	}

	ERROR("Unsupported toolset");
}

function toolset_setup_arch()
{
	if (X64) {
		STDOUT.WriteLine("  Detected 64-bit compiler");
	} else {
		STDOUT.WriteLine("  Detected 32-bit compiler");
	}
	AC_DEFINE('ARCHITECTURE', X64 ? 'x64' : 'x86', "Detected compiler architecture");
	DEFINE("PHP_ARCHITECTURE", X64 ? 'x64' : 'x86');
}

function toolset_setup_codegen_arch()
{
	if("no" == PHP_CODEGEN_ARCH || "yes" == PHP_CODEGEN_ARCH) {
		return;
	}

	if (VS_TOOLSET) {
		var arc = PHP_CODEGEN_ARCH.toUpperCase();

		if ("IA32" != arc) {
			ERROR("Only IA32 arch is supported by --with-codegen-arch, got '" + arc + "'");
		} else if (X64) {
			ERROR("IA32 arch is only supported with 32-bit build");
		}
		ADD_FLAG("CFLAGS", "/arch:" + arc);
		PHP_NATIVE_INTRINSICS = "disabled";
	}
}

function toolset_setup_linker()
{
	var lnk = false;
	if (VS_TOOLSET) {
		lnk = PATH_PROG('link', null);
	} else if (CLANG_TOOLSET) {
		//return PATH_PROG('lld', WshShell.Environment("Process").Item("PATH"), "LINK");
		lnk = PATH_PROG('link', WshShell.Environment("Process").Item("PATH"));
	} else if (ICC_TOOLSET) {
		lnk = PATH_PROG('xilink', WshShell.Environment("Process").Item("PATH"), "LINK");
	}

	if (!lnk) {
		ERROR("Unsupported toolset");
	}

	var ver = probe_binary(lnk);

	var major = ver.substr(0, 2);
	var minor = ver.substr(3, 2);

	AC_DEFINE('PHP_LINKER_MAJOR', major, "Linker major version", false);
	AC_DEFINE('PHP_LINKER_MINOR', minor, "Linker minor version", false);

	return lnk;
}

function toolset_setup_common_cflags()
{
	var envCFLAGS = WshShell.Environment("PROCESS").Item("CFLAGS");

	// CFLAGS for building the PHP dll
	DEFINE("CFLAGS_PHP", "/D _USRDLL /D PHP7DLLTS_EXPORTS /D PHP_EXPORTS \
	/D LIBZEND_EXPORTS /D TSRM_EXPORTS /D SAPI_EXPORTS /D WINVER=" + WINVER);

	DEFINE('CFLAGS_PHP_OBJ', '$(CFLAGS_PHP) $(STATIC_EXT_CFLAGS)');

	// General CFLAGS for building objects
	DEFINE("CFLAGS", "/nologo $(BASE_INCLUDES) /D _WINDOWS /D WINDOWS=1 \
		/D ZEND_WIN32=1 /D PHP_WIN32=1 /D WIN32 /D _MBCS /W3 \
		/D _USE_MATH_DEFINES");

	if (envCFLAGS) {
		ADD_FLAG("CFLAGS", envCFLAGS);
	}

	if (VS_TOOLSET) {
		ADD_FLAG("CFLAGS", " /FD ");

		// fun stuff: MS deprecated ANSI stdio and similar functions
		// disable annoying warnings.  In addition, time_t defaults
		// to 64-bit.  Ask for 32-bit.
		if (X64) {
			ADD_FLAG('CFLAGS', ' /wd4996 ');
		} else {
			ADD_FLAG('CFLAGS', ' /wd4996 /D_USE_32BIT_TIME_T=1 ');
		}

		if (PHP_DEBUG == "yes") {
			// Set some debug/release specific options
			ADD_FLAG('CFLAGS', ' /RTC1 ');
		} else {
			if (PHP_DEBUG == "no" && PHP_SECURITY_FLAGS == "yes") {
				/* Mitigations for CVE-2017-5753.
			  	   TODO backport for all supported VS versions when they release it. */
				if (VCVERS >= 1912) {
					var subver1912 = probe_binary(PHP_CL).substr(6);
					if (VCVERS >= 1913 || 1912 == VCVERS && subver1912 >= 25835) {
						ADD_FLAG('CFLAGS', "/Qspectre");
					} else {
						/* Undocumented. */
						ADD_FLAG('CFLAGS', "/d2guardspecload");
					}
				} else if (1900 == VCVERS) {
					var subver1900 = probe_binary(PHP_CL).substr(6);
					if (subver1900 >= 24241) {
						ADD_FLAG('CFLAGS', "/Qspectre");
					}
				}
			}
			if (VCVERS >= 1900) {
				if (PHP_SECURITY_FLAGS == "yes") {
					ADD_FLAG('CFLAGS', "/guard:cf");
				}
			}
			if (VCVERS >= 1800) {
				if (PHP_PGI != "yes" && PHP_PGO != "yes") {
					ADD_FLAG('CFLAGS', "/Zc:inline");
				}
				/* We enable /opt:icf only with the debug pack, so /Gw only makes sense there, too. */
				if (PHP_DEBUG_PACK == "yes") {
					ADD_FLAG('CFLAGS', "/Gw");
				}
			}
		}

		if (VCVERS >= 1914) {
			/* This is only in effect for CXX sources, __cplusplus is not defined in C sources. */
			ADD_FLAG("CFLAGS", "/Zc:__cplusplus");
		}

		if (VCVERS >= 1914) {
			ADD_FLAG("CFLAGS", "/d2FuncCache1");
		}

		ADD_FLAG("CFLAGS", "/Zc:wchar_t");
	} else if (CLANG_TOOLSET) {
		if (X64) {
			ADD_FLAG('CFLAGS', '-m64');
		} else {
			ADD_FLAG('CFLAGS', '-m32');
		}
		ADD_FLAG("CFLAGS", " /fallback ");
		ADD_FLAG("CFLAGS", "-Xclang -fmodules");

		var vc_ver = probe_binary(PATH_PROG('cl', null));
		ADD_FLAG("CFLAGS"," -fms-compatibility -fms-compatibility-version=" + vc_ver + " -fms-extensions");
	}
}

function toolset_setup_intrinsic_cflags()
{
	var default_enabled = "sse2";
	/* XXX AVX and above needs to be reflected in /arch, for now SSE4.2 is
		the best possible optimization.*/
	var avail = WScript.CreateObject("Scripting.Dictionary");
	avail.Add("sse", "__SSE__");
	avail.Add("sse2", "__SSE2__");
	avail.Add("sse3", "__SSE3__");
	avail.Add("ssse3", "__SSSE3__");
	avail.Add("sse4.1", "__SSE4_1__");
	avail.Add("sse4.2", "__SSE4_2__");
	/* From oldest to newest. */
	var scale = new Array("sse", "sse2", "sse3", "ssse3", "sse4.1", "sse4.2", "avx", "avx2");

	if (VS_TOOLSET) {
		if ("disabled" == PHP_NATIVE_INTRINSICS) {
			ERROR("Can't enable intrinsics, --with-codegen-arch passed with an incompatible option. ")
		}

		if ("no" == PHP_NATIVE_INTRINSICS || "yes" == PHP_NATIVE_INTRINSICS) {
			PHP_NATIVE_INTRINSICS = default_enabled;
		}

		if ("all" == PHP_NATIVE_INTRINSICS) {
			var list = (new VBArray(avail.Keys())).toArray();

			for (var i in list) {
				AC_DEFINE(avail.Item(list[i]), 1);
			}

			/* All means all. __AVX__ and __AVX2__ are defined by compiler. */
			ADD_FLAG("CFLAGS","/arch:AVX2");
			configure_subst.Add("PHP_SIMD_SCALE", "AVX2");
		} else {
			var list = PHP_NATIVE_INTRINSICS.split(",");
			var j = 0;
			for (var k = 0; k < scale.length; k++) {
				for (var i = 0; i < list.length; i++) {
					var it = list[i].toLowerCase();
					if (scale[k] == it) {
						j = k > j ? k : j;
					} else if (!avail.Exists(it) && "avx2" != it && "avx" != it) {
						WARNING("Unknown intrinsic name '" + it + "' ignored");
					}
				}
			}
			if (!X64) {
				/* SSE2 is currently the default on 32-bit. It could change later,
					for now no need to pass it. But, if SSE only was chosen,
					/arch:SSE is required. */
				if ("sse" == scale[j]) {
					ADD_FLAG("CFLAGS","/arch:SSE");
				}
			}
			configure_subst.Add("PHP_SIMD_SCALE", scale[j].toUpperCase());
			/* There is no explicit way to enable intrinsics between SSE3 and SSE4.2.
				The declared macros therefore won't affect the code generation,
				but will enable the guarded code parts. */
			if ("avx2" == scale[j]) {
				ADD_FLAG("CFLAGS","/arch:AVX2");
				j -= 2;
			} else if ("avx" == scale[j]) {
				ADD_FLAG("CFLAGS","/arch:AVX");
				j -= 1;
			}
			for (var i = 0; i <= j; i++) {
				var it = scale[i];
				AC_DEFINE(avail.Item(it), 1);
			}
		}
	}
}

function toolset_setup_common_ldlags()
{
	var envLDFLAGS = WshShell.Environment("PROCESS").Item("LDFLAGS");

	// General DLL link flags
	DEFINE("DLL_LDFLAGS", "/dll ");

	// PHP DLL link flags
	DEFINE("PHP_LDFLAGS", "$(DLL_LDFLAGS)");

	DEFINE("LDFLAGS", "/nologo ");

	if (envLDFLAGS) {
		ADD_FLAG("LDFLAGS", envLDFLAGS);
	}

	// we want msvcrt in the PHP DLL
	ADD_FLAG("PHP_LDFLAGS", "/nodefaultlib:libcmt");

	if (VS_TOOLSET) {
		if (VCVERS >= 1900) {
			if (PHP_SECURITY_FLAGS == "yes") {
				ADD_FLAG('LDFLAGS', "/GUARD:CF");
			}
		}
	}
}

function toolset_setup_common_libs()
{
	// urlmon.lib ole32.lib oleaut32.lib uuid.lib gdi32.lib winspool.lib comdlg32.lib
	DEFINE("LIBS", "kernel32.lib ole32.lib user32.lib advapi32.lib shell32.lib ws2_32.lib Dnsapi.lib psapi.lib bcrypt.lib imagehlp.lib");
}

function toolset_setup_build_mode()
{
	if (PHP_DEBUG == "yes") {
		ADD_FLAG("CFLAGS", "/LDd /MDd /W3 /Od /D _DEBUG /D ZEND_DEBUG=1 " +
			(X64?"/Zi":"/ZI"));
		ADD_FLAG("LDFLAGS", "/debug");
		// Avoid problems when linking to release libraries that use the release
		// version of the libc
		ADD_FLAG("PHP_LDFLAGS", "/nodefaultlib:msvcrt");
	} else {
		// Generate external debug files when --enable-debug-pack is specified
		if (PHP_DEBUG_PACK == "yes") {
			ADD_FLAG("CFLAGS", "/Zi");
			ADD_FLAG("LDFLAGS", "/incremental:no /debug /opt:ref,icf");
		}
		ADD_FLAG("CFLAGS", "/LD /MD /W3");
		if (PHP_SANITIZER == "yes" && CLANG_TOOLSET) {
			ADD_FLAG("CFLAGS", "/Od /D NDebug /D NDEBUG /D ZEND_WIN32_NEVER_INLINE /D ZEND_DEBUG=0");
		} else {
			// Equivalent to Release_TSInline build -> best optimization
			ADD_FLAG("CFLAGS", "/Ox /D NDebug /D NDEBUG /D ZEND_WIN32_FORCE_INLINE /GF /D ZEND_DEBUG=0");
		}

		// if you have VS.Net /GS hardens the binary against buffer overruns
		// ADD_FLAG("CFLAGS", "/GS");
	}
}

function object_out_dir_option_handle()
{
	if (PHP_OBJECT_OUT_DIR.length) {
		PHP_OBJECT_OUT_DIR = FSO.GetAbsolutePathName(PHP_OBJECT_OUT_DIR);
		if (!FSO.FolderExists(PHP_OBJECT_OUT_DIR)) {
			ERROR('chosen output directory ' + PHP_OBJECT_OUT_DIR + ' does not exist');
		}
		PHP_OBJECT_OUT_DIR += '\\';
	} else {
		PHP_OBJECT_OUT_DIR = FSO.GetAbsolutePathName(".") + '\\';

		if (X64) {
			PHP_OBJECT_OUT_DIR += 'x64\\';
			if (!FSO.FolderExists(PHP_OBJECT_OUT_DIR)) {
				FSO.CreateFolder(PHP_OBJECT_OUT_DIR);
			}
		}
	}
}

function setup_zts_stuff()
{
	if (PHP_ZTS == "yes") {
		ADD_FLAG("CFLAGS", "/D ZTS=1");
		ADD_FLAG("ZTS", "1");
	} else {
		ADD_FLAG("ZTS", "0");
	}

	DEFINE("PHP_ZTS_ARCHIVE_POSTFIX", PHP_ZTS == "yes" ? '' : "-nts");

	// set up the build dir and DLL name
	if (PHP_DEBUG == "yes" && PHP_ZTS == "yes") {
		DEFINE("BUILD_DIR", PHP_OBJECT_OUT_DIR + "Debug_TS");
		if (!MODE_PHPIZE) {
			DEFINE("PHPDLL", "php" + PHP_VERSION + "ts_debug.dll");
			DEFINE("PHPLIB", "php" + PHP_VERSION + "ts_debug.lib");
		}
	} else if (PHP_DEBUG == "yes" && PHP_ZTS == "no") {
		DEFINE("BUILD_DIR", PHP_OBJECT_OUT_DIR + "Debug");
		if (!MODE_PHPIZE) {
			DEFINE("PHPDLL", "php" + PHP_VERSION + "_debug.dll");
			DEFINE("PHPLIB", "php" + PHP_VERSION + "_debug.lib");
		}
	} else if (PHP_DEBUG == "no" && PHP_ZTS == "yes") {
		DEFINE("BUILD_DIR", PHP_OBJECT_OUT_DIR + "Release_TS");
		if (!MODE_PHPIZE) {
			DEFINE("PHPDLL", "php" + PHP_VERSION + "ts.dll");
			DEFINE("PHPLIB", "php" + PHP_VERSION + "ts.lib");
		}
	} else if (PHP_DEBUG == "no" && PHP_ZTS == "no") {
		DEFINE("BUILD_DIR", PHP_OBJECT_OUT_DIR + "Release");
		if (!MODE_PHPIZE) {
			DEFINE("PHPDLL", "php" + PHP_VERSION + ".dll");
			DEFINE("PHPLIB", "php" + PHP_VERSION + ".lib");
		}
	}

	if (!FSO.FolderExists(get_define('BUILD_DIR'))) {
		FSO.CreateFolder(get_define('BUILD_DIR'));
	}
}

function php_build_option_handle()
{
	if (PHP_PHP_BUILD == 'no') {
		if (FSO.FolderExists("..\\deps")) {
			PHP_PHP_BUILD = "..\\deps";
		} else {
			if (FSO.FolderExists("..\\php_build")) {
				PHP_PHP_BUILD = "..\\php_build";
			} else {
				if (X64) {
					if (FSO.FolderExists("..\\win64build")) {
						PHP_PHP_BUILD = "..\\win64build";
					} else if (FSO.FolderExists("..\\php-win64-dev\\php_build")) {
						PHP_PHP_BUILD = "..\\php-win64-dev\\php_build";
					}
				} else {
					if (FSO.FolderExists("..\\win32build")) {
						PHP_PHP_BUILD = "..\\win32build";
					} else if (FSO.FolderExists("..\\php-win32-dev\\php_build")) {
						PHP_PHP_BUILD = "..\\php-win32-dev\\php_build";
					}
				}
			}
		}
		PHP_PHP_BUILD = FSO.GetAbsolutePathName(PHP_PHP_BUILD);
	}
	DEFINE("PHP_BUILD", PHP_PHP_BUILD);
}

// Poke around for some headers
function probe_basic_headers()
{
	var p;

	if (PHP_PHP_BUILD != "no") {
		php_usual_include_suspects += ";" + PHP_PHP_BUILD + "\\include";
		php_usual_lib_suspects += ";" + PHP_PHP_BUILD + "\\lib";
	}
}

function add_extra_dirs()
{
	var path, i, f;

	if (PHP_EXTRA_INCLUDES.length) {
		path = PHP_EXTRA_INCLUDES.split(';');
		for (i = 0; i < path.length; i++) {
			f = FSO.GetAbsolutePathName(path[i]);
			if (FSO.FolderExists(f)) {
				ADD_FLAG("CFLAGS", '/I "' + f + '" ');
			}
		}
	}
	if (PHP_EXTRA_LIBS.length) {
		path = PHP_EXTRA_LIBS.split(';');
		for (i = 0; i < path.length; i++) {
			f = FSO.GetAbsolutePathName(path[i]);
			if (FSO.FolderExists(f)) {
				if (VS_TOOLSET && VCVERS <= 1200 && f.indexOf(" ") >= 0) {
					ADD_FLAG("LDFLAGS", '/libpath:"\\"' + f + '\\"" ');
					ADD_FLAG("ARFLAGS", '/libpath:"\\"' + f + '\\"" ');
				} else {
					ADD_FLAG("LDFLAGS", '/libpath:"' + f + '" ');
					ADD_FLAG("ARFLAGS", '/libpath:"' + f + '" ');
				}
			}
		}
	}

}

function trim(s)
{
	return s.replace(/^\s+/, "").replace(/\s+$/, "");
}

function force_all_shared()
{
	return !!PHP_ALL_SHARED && "yes" == PHP_ALL_SHARED;
}

function ADD_MAKEFILE_FRAGMENT(src_file)
{
	var fn_in;

	if ("undefined" == typeof(src_file)) {
		fn_in = configure_module_dirname + "\\Makefile.frag.w32";
	} else {
		fn_in = src_file;
	}

	if (FSO.FileExists(fn_in)) {
		var h_in, h_out;
		var create_out_fl = !FSO.FileExists(PHP_MAKEFILE_FRAGMENTS);
		var open_flags = create_out_fl ? 2 : 8;

		h_in = FSO.OpenTextFile(fn_in, 1);
		h_out = FSO.OpenTextFile(PHP_MAKEFILE_FRAGMENTS, open_flags, create_out_fl);

		if (!h_in.AtEndOfStream) {
			h_out.Write(h_in.ReadAll());
			h_out.WriteBlankLines(1);
		}

		h_in.Close();
		h_out.Close();
	}
}

function SETUP_OPENSSL(target, path_to_check, common_name, use_env, add_dir_part, add_to_flag_only)
{
	var ret = 0;
	var cflags_var = "CFLAGS_" + target.toUpperCase();

	if (CHECK_LIB("libcrypto.lib", target, path_to_check) &&
			CHECK_LIB("libssl.lib", target, path_to_check) &&
			CHECK_LIB("crypt32.lib", target, path_to_check, common_name) &&
			CHECK_HEADER_ADD_INCLUDE("openssl/ssl.h", cflags_var, path_to_check, use_env, add_dir_part, add_to_flag_only)) {
		/* Openssl 1.1.x */
		return 2;
	} else if (CHECK_LIB("ssleay32.lib", target, path_to_check, common_name) &&
			CHECK_LIB("libeay32.lib", target, path_to_check, common_name) &&
			CHECK_LIB("crypt32.lib", target, path_to_check, common_name) &&
			CHECK_HEADER_ADD_INCLUDE("openssl/ssl.h", cflags_var, path_to_check, use_env, add_dir_part, add_to_flag_only)) {
		/* Openssl 1.0.x and lower */
		return 1;
	}

	return ret;
}

function SETUP_SQLITE3(target, path_to_check, shared) {
	var cflags_var = "CFLAGS_" + target.toUpperCase();
	var libs = (shared ? "libsqlite3.lib;libsqlite3_a.lib" : "libsqlite3_a.lib;libsqlite3.lib");

	return CHECK_LIB(libs, target, path_to_check) &&
		CHECK_HEADER_ADD_INCLUDE("sqlite3.h", cflags_var) &&
		CHECK_HEADER_ADD_INCLUDE("sqlite3ext.h", cflags_var);
}

function check_binary_tools_sdk()
{
	var BIN_TOOLS_SDK_VER_MAJOR = 0;
	var BIN_TOOLS_SDK_VER_MINOR = 0;
	var BIN_TOOLS_SDK_VER_PATCH = 0;

	var out = execute("cmd /c phpsdk_version");

	if (out.match(/PHP SDK (\d+)\.(\d+)\.(\d+).*/)) {
		BIN_TOOLS_SDK_VER_MAJOR = parseInt(RegExp.$1);
		BIN_TOOLS_SDK_VER_MINOR = parseInt(RegExp.$2);
		BIN_TOOLS_SDK_VER_PATCH = parseInt(RegExp.$3);
	}

	/* Basic test, extend by need. */
	if (BIN_TOOLS_SDK_VER_MAJOR < 2) {
		ERROR("Incompatible binary tools version. Please consult\r\nhttps://wiki.php.net/internals/windows/stepbystepbuild_sdk_2");
	}
}

function get_clang_lib_dir()
{
	var ret = null;
	var ver = null;

	if (COMPILER_NAME_LONG.match(/clang version ([\d\.]+) \((.*)\)/)) {
		ver = RegExp.$1;
	} else {
		ERROR("Faled to determine clang lib path");
	}

	if (X64) {
		ret = PROGRAM_FILES + "\\LLVM\\lib\\clang\\" + ver + "\\lib";
		if (!FSO.FolderExists(ret)) {
			ret = null;
		}
	} else {
		ret = PROGRAM_FILESx86 + "\\LLVM\\lib\\clang\\" + ver + "\\lib";
		if (!FSO.FolderExists(ret)) {
			ret = PROGRAM_FILES + "\\LLVM\\lib\\clang\\" + ver + "\\lib";
			if (!FSO.FolderExists(ret)) {
				ret = null;
			}
		}
	}

	if (null == ret) {
		ERROR("Invalid clang lib path encountered");
	}

	return ret;
}

function add_asan_opts(cflags_name, libs_name, ldflags_name)
{

	var ver = null;

	if (COMPILER_NAME_LONG.match(/clang version ([\d\.]+) \((.*)\)/)) {
		ver = RegExp.$1;
	} else {
		ERROR("Faled to determine clang lib path");
	}

	if (!!cflags_name) {
		ADD_FLAG(cflags_name, "-fsanitize=address,undefined");
	}
	if (!!libs_name) {
		if (X64) {
			ADD_FLAG(libs_name, "clang_rt.asan_dynamic-x86_64.lib clang_rt.asan_dynamic_runtime_thunk-x86_64.lib");
		} else {
			ADD_FLAG(libs_name, "clang_rt.asan_dynamic-i386.lib clang_rt.asan_dynamic_runtime_thunk-i386.lib");
		}
	}

	if (!!ldflags_name) {
		ADD_FLAG(ldflags_name, "/libpath:\"" + get_clang_lib_dir() + "\\windows\"");
	}
}

function setup_verbosity()
{
	if ("no" != PHP_VERBOSITY) {
		if ("yes" == PHP_VERBOSITY) {
			VERBOSITY = 1;
			if (1 == VERBOSITY) {
				CMD_MOD1 = "";
			}
		} else {
			var _tmp = parseInt(PHP_VERBOSITY);
			if (0 != _tmp && 1 != _tmp && 2 != _tmp) {
				ERROR("Unsupported verbosity level '" + PHP_VERBOSITY + "'");
			}
			VERBOSITY = _tmp;

			if (1 == VERBOSITY) {
				CMD_MOD1 = "";
			}
			if (2 == VERBOSITY) {
				CMD_MOD1 = "";
				CMD_MOD2 = "";
			}
		}
	} else {
		VERBOSITY = 0;
		CMD_MOD1 = "@";
		CMD_MOD2 = "@";
	}
}

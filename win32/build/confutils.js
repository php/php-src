// Utils for configure script
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@thebrainroom.com>                           |
  +----------------------------------------------------------------------+
*/

// $Id: confutils.js,v 1.12 2003-12-04 02:12:36 wez Exp $

var STDOUT = WScript.StdOut;
var STDERR = WScript.StdErr;
var WshShell = WScript.CreateObject("WScript.Shell");
var FSO = WScript.CreateObject("Scripting.FileSystemObject");
var MFO = null;
var SYSTEM_DRIVE = WshShell.Environment("Process").Item("SystemDrive");
var PROGRAM_FILES = WshShell.Environment("Process").Item("ProgramFiles");

if (PROGRAM_FILES == null) {
	PROGRAM_FILES = "C:\\Program Files\\";
}

STDOUT.WriteLine("program files " + PROGRAM_FILES);

var PHP_VERSION = 5;

configure_args = new Array();
configure_subst = WScript.CreateObject("Scripting.Dictionary");

configure_hdr = WScript.CreateObject("Scripting.Dictionary");
build_dirs = new Array();

extension_include_code = "";
extension_module_ptrs = "";

function ConfigureArg(type, optname, helptext, defval)
{
	var opptype = type == "enable" ? "disable" : "without";

	if (defval == "yes") {
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
	} else if (argval.match(new RegExp("^shared,(.*)"))) {
		shared = true;
		argval = $1;
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
	
	args = WScript.Arguments;
	for (i = 0; i < args.length; i++) {
		arg = args(i);
		nice += ' "' + arg + '"';
		if (arg == "--help") {
			configure_help_mode = true;
			break;
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
				shared = analyzed[0];
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

	// Now set any defaults we might have missed out earlier
	for (i = 0; i < configure_args.length; i++) {
		arg = configure_args[i];
		if (arg.seen)
			continue;
		analyzed = analyze_arg(arg.defval);
		shared = analyzed[0];
		argval = analyzed[1];
		eval("PHP_" + arg.symval + " = argval;");
		eval("PHP_" + arg.symval + "_SHARED = shared;");
	}

	MFO = FSO.CreateTextFile("Makefile.objects", true);

	STDOUT.WriteLine("Saving configure options to config.nice.bat");
	var nicefile = FSO.CreateTextFile("config.nice.bat", true);
	nicefile.WriteLine(nice);
	nicefile.Close();

	AC_DEFINE('CONFIGURE_COMMAND', nice);
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

	if (explicit_path != null) {
		if (typeof(explicit_path) == "string") {
			explicit_path = explicit_path.split(";");
		}

		for (i = 0; i < explicit_path.length; i++) {
			file = FSO.BuildPath(FSO.GetAbsolutePathName(explicit_path[i]), thing_to_find);
			if (FSO.FileExists(file)) {
				found = true;
				place = explicit_path[i];
				break;
			}
		}
	}

	if (!found && env_name != null) {
		env = WshShell.Environment("Process").Item(env_name);
		env = env.split(";");
		for (i = 0; i < env.length; i++) {
			file = FSO.BuildPath(env[i], thing_to_find);
			if (FSO.FileExists(file)) {
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

function PATH_PROG(progname, def, additional_paths)
{
	var exe;
	var place;

	exe = progname + ".exe";

	place = search_paths(exe, additional_paths, "PATH");

	if (place == true) {
		place = exe;
	}

	if (place) {
		DEFINE(progname.toUpperCase(), place);
	}
	return place;
}

function CHECK_LIB(libname, target, path_to_check)
{
	if (path_to_check == null) {
		path_to_check = php_usual_lib_suspects;
	} else {
		path_to_check += ";" + php_usual_lib_suspects;
	}
	
	var p = search_paths(libname, path_to_check, "LIB");
	var have = 0;

	if (typeof(p) == "string") {
		ADD_FLAG("LDFLAGS_" + target.toUpperCase(), '/libpath:"' + p + '" ');
		ADD_FLAG("LIBS_" + target.toUpperCase(), libname);
		have = 1;
	} else if (p == true) {
		ADD_FLAG("LIBS_" + target.toUpperCase(), libname);
		have = 1;
	} else {
		/* not found in the defaults or the explicit paths,
		 * so check the general extra libs; if we find
		 * it here, no need to add another /libpath: for it as we
		 * already have it covered, but we need to add the lib
		 * to LIBS_XXX */
		if (false != search_paths(libname, PHP_EXTRA_LIBS, null)) {
			ADD_FLAG("LIBS_" + target.toUpperCase(), libname);
			have = 1;
		}
	}

//	AC_DEFINE("HAVE_" + header_name.toUpperCase().replace(new RegExp("/\\\\-\.", "g"), "_"), have);

	return p;

}

function CHECK_HEADER_ADD_INCLUDE(header_name, flag_name, path_to_check, use_env)
{
	if (use_env == null) {
		use_env = true;
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
		ADD_FLAG(flag_name, '/I "' + p + '" ');
		have = 1;
	} else if (p == false) {
		/* not found in the defaults or the explicit paths,
		 * so check the general extra includes; if we find
		 * it here, no need to add another /I for it as we
		 * already have it covered */
		p = search_paths(header_name, PHP_EXTRA_INCLUDES, null);
	}

	sym = header_name.toUpperCase();
	sym = sym.replace(new RegExp("[\\\\/\.-]", "g"), "_");

	AC_DEFINE("HAVE_" + sym, have);

	return p;
}

/* emits rule to generate version info for a SAPI
 * or extension.  Returns the name of the .res file
 * that will be generated */
function generate_version_info_resource(makefiletarget, creditspath)
{
	var resname = makefiletarget + ".res";
	var res_desc = "PHP " + makefiletarget;
	var res_prod_name = res_desc;
	var credits;
	var thanks = "";

	if (FSO.FileExists(creditspath + '/CREDITS')) {
		credits = FSO.OpenTextFile(creditspath + '/CREDITS', 1);
		res_desc = credits.ReadLine();
		thanks = credits.ReadLine();
		if (thanks == null) {
			thanks = "";
		} else {
			thanks = "Thanks to " + thanks;
		}
		credits.Close();
	}
	
	MFO.WriteLine("$(BUILD_DIR)\\" + resname + ": win32\\build\\template.rc");
	MFO.WriteLine("\t$(RC) /fo $(BUILD_DIR)\\" + resname +
	   	' /d FILE_DESCRIPTION="\\"' + res_desc + '\\"" /d FILE_NAME="\\"' + makefiletarget +
	   	'\\"" /d PRODUCT_NAME="\\"' + res_prod_name + '\\"" /d THANKS_GUYS="\\"' +
		thanks + '\\"" win32\\build\\template.rc');
	MFO.WriteBlankLines(1);
	
	return resname;
}

function SAPI(sapiname, file_list, makefiletarget, cflags)
{
	var SAPI = sapiname.toUpperCase();
	var ldflags;
	var resname;

	STDOUT.WriteLine("Enabling sapi/" + sapiname);

	MFO.WriteBlankLines(1);
	MFO.WriteLine("# objects for SAPI " + sapiname);
	MFO.WriteBlankLines(1);

	if (cflags) {
		ADD_FLAG('CFLAGS_' + SAPI, cflags);
	}

	ADD_SOURCES("sapi/" + sapiname, file_list, sapiname);
	MFO.WriteBlankLines(1);
	MFO.WriteLine("# SAPI " + sapiname);
	MFO.WriteBlankLines(1);

	/* generate a .res file containing version information */
	resname = generate_version_info_resource(makefiletarget, "sapi/" + sapiname);
	
	MFO.WriteLine(makefiletarget + ": $(BUILD_DIR)\\" + makefiletarget);
	MFO.WriteLine("\t@echo SAPI " + sapiname + " build complete");
	MFO.WriteLine("$(BUILD_DIR)\\" + makefiletarget + ": $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname);

	if (makefiletarget.match(new RegExp("\\.dll$"))) {
		ldflags = "/dll $(LDFLAGS)";
	} else {
		ldflags = "$(LDFLAGS)";
	}
	
	MFO.WriteLine("\t$(LD) /nologo /out:$(BUILD_DIR)\\" + makefiletarget + " " + ldflags + " $(" + SAPI + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(LDFLAGS_" + SAPI + ") $(LIBS_" + SAPI + ") $(BUILD_DIR)\\" + resname);

	DEFINE('CFLAGS_' + SAPI + '_OBJ', '$(CFLAGS_' + SAPI + ')');
	ADD_FLAG("SAPI_TARGETS", makefiletarget);
	MFO.WriteBlankLines(1);
}

function file_get_contents(filename)
{
	var f, c;
	f = FSO.OpenTextFile(filename, 1);
	c = f.ReadAll();
	f.Close();
	return c;
}

function EXTENSION(extname, file_list, shared, cflags)
{
	var objs = null;
	var EXT = extname.toUpperCase();
	var dllname = false;

	if (shared == null) {
		eval("shared = PHP_" + EXT + "_SHARED;");
	}
	if (cflags == null) {
		cflags = "";
	}

	if (shared) {
		STDOUT.WriteLine("Enabling ext/" + extname + " [shared]");
		cflags = "/D COMPILE_DL_" + EXT + " /D " + EXT + "_EXPORTS=1 " + cflags;
		ADD_FLAG("CFLAGS_PHP", "/D COMPILE_DL_" + EXT);
	} else {
		STDOUT.WriteLine("Enabling ext/" + extname);
	}

	MFO.WriteBlankLines(1);
	MFO.WriteLine("# objects for EXT " + extname);
	MFO.WriteBlankLines(1);


	ADD_SOURCES("ext/" + extname, file_list, extname);
	
	MFO.WriteBlankLines(1);

	if (shared) {
		dllname = "php_" + extname + ".dll";
		var resname = generate_version_info_resource(dllname, "ext/" + extname);
	
		MFO.WriteLine("$(BUILD_DIR)\\" + dllname + ": $(" + EXT + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(BUILD_DIR)\\" + resname);
		MFO.WriteLine("\t$(LD) /out:$(BUILD_DIR)\\" + dllname + " $(DLL_LDFLAGS) $(LDFLAGS) $(LDFLAGS_" + EXT + ") $(" + EXT + "_GLOBAL_OBJS) $(BUILD_DIR)\\$(PHPLIB) $(LIBS_" + EXT + ") $(LIBS) $(BUILD_DIR)\\" + resname);
		MFO.WriteBlankLines(1);

		ADD_FLAG("EXT_TARGETS", dllname);
		MFO.WriteLine(dllname + ": $(BUILD_DIR)\\" + dllname);
		MFO.WriteLine("\t@echo EXT " + extname + " build complete");
		MFO.WriteBlankLines(1);
		
		DEFINE('CFLAGS_' + EXT + '_OBJ', '$(CFLAGS_' + EXT + ')');
	} else {
		ADD_FLAG("STATIC_EXT_OBJS", "$(" + EXT + "_GLOBAL_OBJS)");
		ADD_FLAG("STATIC_EXT_LIBS", "$(LIBS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_LDFLAGS", "$(LDFLAGS_" + EXT + ")");
		ADD_FLAG("STATIC_EXT_CFLAGS", "$(CFLAGS_" + EXT + ")");

		/* find the header that declars the module pointer,
		 * so we can include it in internal_functions.c */
		var ext_dir = FSO.GetFolder("ext/" + extname);
		var fc = new Enumerator(ext_dir.Files);
		var re = /\.h$/;
		var s, c;
		for (; !fc.atEnd(); fc.moveNext()) {
			s = fc.item() + "";
			if (s.match(re)) {
				c = file_get_contents(s);
				if (c.match("phpext_")) {
					extension_include_code += '#include "ext/' + extname + '/' + FSO.GetFileName(s) + '"\r\n';
				}
			}
		}
	
		extension_module_ptrs += '\tphpext_' + extname + '_ptr,\r\n';
	
		DEFINE('CFLAGS_' + EXT + '_OBJ', '$(CFLAGS_PHP) $(CFLAGS_' + EXT + ')');
	}
	ADD_FLAG("CFLAGS_" + EXT, cflags);
}

function ADD_SOURCES(dir, file_list, target)
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

	if (target != "php") {
		build_dirs[build_dirs.length] = target;
		sub_build += target + "\\";
	}
	DEFINE("CFLAGS_BD_" + target.toUpperCase(), "/Fo" + sub_build + " /Fd" + sub_build + " /Fp" + sub_build + " /FR" + sub_build + " ");

	for (i in file_list) {
		src = file_list[i];
		obj = src.replace(re, ".obj");
		tv += " " + sub_build + obj;

		if (PHP_ONE_SHOT == "yes") {
			if (i > 0) {
				objs_line += " " + sub_build + obj;	
				srcs_line += " " + dir + "\\" + src;
			} else {
				objs_line = sub_build + obj;	
				srcs_line = dir + "\\" + src;
			}
		} else {
			MFO.WriteLine(sub_build + obj + ": " + dir + "\\" + src);
			MFO.WriteLine("\t$(CC) $(CFLAGS) $(" + flags + ") $(CFLAGS_BD_" + target.toUpperCase() + ") -c " + dir + "\\" + src + " -o " + sub_build + obj);
		}
	}

	if (PHP_ONE_SHOT == "yes") {
		MFO.WriteLine(objs_line + ": " + srcs_line);
		MFO.WriteLine("\t$(CC) $(CFLAGS) $(" + flags + ") $(CFLAGS_BD_" + target.toUpperCase() + ") -c " + srcs_line);
	}

	DEFINE(sym, tv);
}

function generate_internal_functions()
{
	var infile, outfile;
	var indata;

	STDOUT.WriteLine("Generating main/internal_functions.c");
	
	infile = FSO.OpenTextFile(WshShell.CurrentDirectory + "/main/internal_functions.c.in", 1);
	indata = infile.ReadAll();
	infile.Close();
	
	outfile = FSO.CreateTextFile(WshShell.CurrentDirectory + "/main/internal_functions.c", true);

	indata = indata.replace("@EXT_INCLUDE_CODE@", extension_include_code);
	indata = indata.replace("@EXT_MODULE_PTRS@", extension_module_ptrs);

	outfile.Write(indata);
	outfile.Close();
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
		ADD_FLAG("BUILD_DIRS_SUB", bd);
		if (!FSO.FolderExists(bd)) {
			FSO.CreateFolder(bd);
		}
	}
		
	STDOUT.WriteLine("Generating files...");
	generate_makefile();
	generate_internal_functions();
	generate_config_h();


	STDOUT.WriteLine("Done.");
	STDOUT.WriteBlankLines(1);
	STDOUT.WriteLine("Type 'nmake' to build PHP");
}

function generate_config_h()
{
	var infile, outfile;
	var indata;
	var prefix;

	prefix = PHP_PREFIX.replace("\\", "\\\\");

	STDOUT.WriteLine("Generating main/config.w32.h");
	
	infile = FSO.OpenTextFile(WshShell.CurrentDirectory + "/win32/build/config.w32.h.in", 1);
	indata = infile.ReadAll();
	infile.Close();
	
	outfile = FSO.CreateTextFile(WshShell.CurrentDirectory + "/main/config.w32.h", true);

	indata = indata.replace(new RegExp("@PREFIX@", "g"), prefix);
	outfile.Write(indata);

	var keys = (new VBArray(configure_hdr.Keys())).toArray();
	var i;
	var item;

	outfile.WriteBlankLines(1);
	outfile.WriteLine("/* values determined by configure.js */");

	for (i in keys) {
		item = configure_hdr.Item(keys[i]);
		outfile.WriteBlankLines(1);
		outfile.WriteLine("/* " + item[1] + " */");
		outfile.WriteLine("#define " + keys[i] + " " + item[0]);
	}
	
	outfile.Close();
}

function generate_makefile()
{
	STDOUT.WriteLine("Generating Makefile");
	var MF = FSO.CreateTextFile("Makefile", true);

	MF.WriteLine("# Generated by configure.js");

	/* spit out variable definitions */
	var keys = (new VBArray(configure_subst.Keys())).toArray();
	var i;

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

	var TF = FSO.OpenTextFile("win32/build/Makefile", 1);
	MF.Write(TF.ReadAll());
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

		if (curr_flags.match(flags)) {
			return;
		}
		
		flags = curr_flags + " " + flags;
		configure_subst.Remove(name);
	}
	configure_subst.Add(name, flags);
}

function get_define(name)
{
	return configure_subst.Item(name);
}

// Add a .def to the core to export symbols
function ADD_DEF_FILE(name)
{
	if (!configure_subst.Exists("PHPDEF")) {
		DEFINE("PHPDEF", "win32\\phpts.def");
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
	configure_hdr.Add(name, item);
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


/* check for duplicate entries */
function check_duplicates(local, core)
{
	if (!local) {
		return core;
	}

	arr = local.split(" ");

	for(i = 0; i < arr.length; i++) {
		if (core.match(arr[i])) {
			continue;
		}
		core += " " + arr[i];
	}

	return core;
}

/* read .dsp source blocks */
function read_src_files(ext, tmpl, path)
{
	sources = file_get_contents("tmp\\src\\" + ext + ".sources.tmp");
	sources = (path ? sources.replace(/\.\//g, path) : sources);
	tmpl = tmpl.replace("SOURCEFILES", sources);
	FSO.DeleteFile("tmp\\src\\" + ext + ".sources.tmp");

	headers = file_get_contents("tmp\\src\\" + ext + ".headers.tmp");
	headers = (path ? headers.replace(/\.\//g, path) : headers);
	tmpl = tmpl.replace("HEADERFILES", headers);
	FSO.DeleteFile("tmp\\src\\" + ext + ".headers.tmp");

	return tmpl;
}

/* write a .dsp source block */
function write_src_file(fname, path, intpath, arr)
{
	FSO.FolderExists("tmp\\src") ? "" : FSO.CreateFolder("tmp\\src");
	var src = FSO.CreateTextFile("tmp\\src\\" + fname, true);
	var out = get_define("BUILD_DIR");
	var libpath = "";

	for (i = 0; i < arr.length; i++) {
		if (arr[i].length > 1) {
			if (arr[i].match('alloca.c') ||
				arr[i].match(/internal_functions_(nw|win32)\.c/) ||
				arr[i].match(/flock\.(c|h)/) ||
				arr[i].match(/zend_static_allocator\.(c|h)/) ||
				arr[i].match(/zend_(ini|language)_scanner_defs\.h/)) {
				continue;
			}

			libpath = arr[i].substr(2, arr[i].lastIndexOf("\\") - 2);
			if (libpath) {
				libpath = "\\" + libpath;
			}

			src.WriteLine("# Begin Source File");
			src.WriteLine("SOURCE=" + arr[i]);
			src.WriteLine('# PROP Intermediate_Dir "' + intpath + out + '\\' + path + libpath + '"');
			src.WriteLine("# End Source File");
			src.WriteBlankLines(1);
		}
	}

	src.Close();
	return;
}

/* generate list of text files */
function generate_text_filelist(ext, ext_dir)
{
	var txtdir = FSO.GetFolder(ext_dir);

	block = '# Begin Group "Text Files"\r\n\# PROP Default_Filter ""\r\n\r\n';
	txt = new Enumerator(txtdir.Files);

	for (; !txt.atEnd(); txt.moveNext()) {
		fname = FSO.GetFileName(txt.item());
		munged = fname.replace(ext, ""); /* TSRM...! */

		if (munged.match(/[A-Z]{4}/)){
			block += "# Begin Source File\r\n";
			block += "SOURCE=./" + fname + "\r\n";
			block += "# End Source File\r\n\r\n";
		}
	}

	block += "# End Group\r\n";
	return block;
}

/* generate list of resource files */
function generate_resource_filelist(ext, ext_dir)
{
	var resdir = FSO.GetFolder(ext_dir);
	res = new Enumerator(resdir.Files);
	block = "";

	for (; !res.atEnd(); res.moveNext()) {
		fname = FSO.GetFileName(res.item());

		if (fname.match(/\.(ico|rc)/)) {
			block += "# Begin Source File\r\n";
			block += "SOURCE=./" + fname + "\r\n";
			block += "# End Source File\r\n\r\n";
		}
	}

	return block;
}

/* generate parser and scanner files for Zend */
function generate_parsers_or_scanners(arr, type)
{
	var filter = (type.match("Parsers") ? "y" : "l");

	ret = '# Begin Group "' + type + '"\r\n# PROP Default_Filter "' + filter + '"\r\n\r\n';

	for (i = 0; i < arr.length; i++) {

		fl = "zend_" + arr[i] + "_" + type.toLowerCase().substr(0, type.length - 1);
		ret += "# Begin Source File\r\n";
		ret += "SOURCE=.\\" + fl + "." + filter + "\r\n\r\n";
		ret += '# Begin Custom Build\r\n\r\n';

		if (type.match("Parsers")) {
			pre = (arr[i].match(/ini/) ? "ini_ " : "zend ");
			ret += fl + ".c " + fl + ".h: " + fl + ".y\r\n";
			ret += "\tbison --output=" + fl + ".c -v -d -p " + pre + fl + ".y\r\n\r\n";
		} else {
			ret += fl + ".c: " + fl + ".l\r\n";
			ret += "\tre2c --case-inverted -cbdFt " + fl + "_defs.h -o" + fl + ".c " + fl + ".l\r\n\r\n";
		}

		ret += "# End Custom Build\r\n";
		ret += "# End Source File\r\n";
	}

	ret += "# End Group\r\n\r\n";
	return ret;
}

/* generate .defs file for php5[ts].dll */
function generate_php_defs()
{
	var defs = get_define("PHP_DLL_DEF_SOURCES").split(" ");
	var bdir = get_define("BUILD_DIR") + "\\";
	var file = get_define("PHPLIB").replace("lib", "def");
	var path = "..\\" + bdir + file;
	var deps = "USERDEP__PHP5TS=";
	var cmds = "BuildCmds= \\\r\n";
	var cmd = '$(SOURCE) "$(INTDIR)" "$(OUTDIR)"\r\n\t$(BuildCmds)\r\n';

	for (i = 0; i < defs.length; i++) {
		deps += '"..\\' + defs[i] + '" ';
		cmds += "\ttype ..\\" + defs[i] + (i == 0 ? " > " : " >> ") + path + " \\\r\n";
	}

	ret = '# Begin Group "Defs Files"\r\n\r\n';
	ret += "# Begin Source File\r\nSOURCE=" + path + "\r\n\r\n";
	ret += deps.substr(0, deps.length-1) + "\r\n# Begin Custom Build - ";
	ret += "Generating $(InputPath)\r\nInputPath=" + path + "\r\n\r\n";
	ret += cmds + '\r\n\"' + path + '" : ' + cmd + "\r\n";
	ret += "# End Custom Build\r\n# End Source File\r\n\r\n";
	ret += "# End Group\r\n";
	return ret;
}

/* generate win32\wsyslog.h for php5[ts].dll */
function generate_wsyslog()
{
	var path = ".\\build\\wsyslog.mc\r\n\r\n";
	var intdir = "..\\" + get_define("BUILD_DIR");

	ret = "# Begin Source File\r\nSOURCE=" + path;
	ret += "# Begin Custom Build\r\nInputDir=.\\build\r\n";
	ret += "IntDir=" + intdir + "\r\nInputPath=" + path;
	ret += '"wsyslog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\r\n';
	ret += "\tmc -h $(InputDir)/.. -r $(InputDir) -x $(IntDir) $(InputPath)\r\n\r\n";
	ret += "# End Custom Build\r\n# End Source File\r\n";
	return ret;
}

/* generate ext\date\lib\timelib_config.h for php5[ts].dll */
function generate_timelib_conf(headers)
{
	var file = "timelib_config.h";
	var path = "..\\ext\\date\\lib\\timelib_config.h";
	var pos = headers.search(file);
	var entry = headers.slice(pos, pos + 64);

	replace = entry.replace(file, file + ".win32");
	replace += "\r\n\r\n# Begin Custom Build\r\nInputDir=..\\ext\\date\\lib\r\n";
	replace += "InputPath=" + path + ".win32\r\n\r\n";
	replace += '"' + path + '" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\r\n';
	replace += "\tcopy $(InputPath) $(InputDir)\\" + file + "\r\n\r\n";
	replace += "# End Custom Build";

	headers = headers.replace(entry, replace);
	return headers;
}

/* generate php5[ts].dsp */
function generate_core_dsp(core_headers, core_sources, headers, sources, cflags, ldflags, libs)
{
	var ts = (PHP_ZTS != "no" ? "ts" : "");
	var extname = "php5" + ts;
	var tmpl = generate_dsp_file(extname, ".", false, false);

	cflags += get_define("CFLAGS_PHP").replace("/D _USRDLL", "");
	cflags = cflags.replace(/\/(I|D)(\S)/g, "/$1 $2");
	ldflags += get_define("LDFLAGS_PHP");
	libs += get_define("LIBS_PHP");

	tmpl = tmpl.replace(/LOCALCPP/, cflags.replace(/\"ext/g, '"../ext') + " /c");
	tmpl = tmpl.replace(/LOCALLIBS/, libs);
	tmpl = tmpl.replace(/LOCALLDFLAGS/, ldflags);
	tmpl = tmpl.replace(extname + ".dll", get_define("PHPDLL"));

	wsyslog = (core_headers.match("wsyslog.h") ? "" : generate_wsyslog(core_headers));
	core_sources = '# Begin Group "CORE"\r\n' + core_sources + "# End Group\r\n";
	tmpl = tmpl.replace(/CORESOURCES/, core_sources);
	core_headers = '# Begin Group "CORE "\r\n' + core_headers + "# End Group\r\n";
	tmpl = tmpl.replace(/COREHEADERS/, core_headers + wsyslog);

	headers = generate_timelib_conf(headers);
	tmpl = tmpl.replace(/SOURCEFILES/, sources);
	tmpl = tmpl.replace(/HEADERFILES/, headers);

	defs = generate_php_defs();
	tmpl = tmpl.replace(/DEFS/, defs);

	dsp = FSO.CreateTextFile("win32\\php5" + ts + ".dsp", true);
	STDOUT.WriteLine("\tGenerating win32\\php5" + ts + ".dsp");
	dsp.Write(tmpl);
	dsp.Close();

	return;
}

/* generate .dsw files */
function generate_dsw_files(sblocks, mblocks)
{
	var stmpl = file_get_contents("win32\\build\\template.dsw");
	var mtmpl = file_get_contents("win32\\build\\template.dsw");
	var ts = (PHP_ZTS != "no" ? "ts" : "");

	/* push all the sapi blocks to the same tag */
	stmpl = stmpl.replace("INSERT", sblocks);
	stmpl = (PHP_ZTS != "no" ? stmpl : stmpl.replace(/dllts/g, "dll"));
	sdsw = FSO.CreateTextFile("win32\\php5" + ts + ".dsw", true);
	STDOUT.WriteLine("\tGenerating win32\\php5" + ts + ".dsw");
	sdsw.Write(stmpl);
	sdsw.Close();

	/* same for shared modules - except that nothing else goes in here */
	garbage = mtmpl.slice(200, mtmpl.search("INSERT"));
	mtmpl = mtmpl.replace(garbage, "\r\n");
	mtmpl = mtmpl.replace("INSERT", mblocks);
	mtmpl = (PHP_ZTS != "no" ? mtmpl : mtmpl.replace(/dllts/g, "dll"));
	mdsw = FSO.CreateTextFile("win32\\php_modules.dsw", true);
	STDOUT.WriteLine("\tGenerating win32\\php_modules.dsw");
	mdsw.Write(mtmpl);
	mdsw.Close();

	return;
}

/* finalize .dsp files and copy to final destination */
function copy_dsp_files()
{
	var tmp = FSO.GetFolder("tmp");
	var CORE_HEADERS = "";
	var CORE_SOURCES = "";
	var EXT_HEADERS = "";
	var EXT_SOURCES = "";
	var EXT_CFLAGS = "";
	var EXT_LDFLAGS = "";
	var EXT_LIBS = "";
	var sblocks = ""; /* for sapis */
	var mblocks = ""; /* for modules */

	f = new Enumerator(tmp.Files);

	for (; !f.atEnd(); f.moveNext()) {
		/* retrieve the path */
		contents = file_get_contents(f.item());
		address = contents.slice(0, contents.indexOf("#"));
		contents = contents.slice(contents.indexOf("#")+1);
		shared = contents.slice(0, contents.indexOf("#"));
		contents = contents.slice(contents.indexOf("#"));

		/* pick up module name and path */
		path = address.slice(0, address.lastIndexOf("\\")+1);
		ext = address.slice(address.lastIndexOf("\\")+1, address.length-4);
		EXT = ext.toUpperCase();

		if (path.match(/(sapi|ext)/)) {
			rel = "..\\..\\";
		} else {
			rel = "..\\";
		}

		/* pick up local flags and libs */
		cflags = get_define("CFLAGS_" + EXT);
		cflags += (ext.match(/(TSRM|Zend)/) ? "/D TSRM_EXPORTS " : "");
		cflags += (ext.match(/Zend/) ? "/D LIBZEND_EXPORTS " : "");
		libs = get_define("LIBS_" + EXT);
		ldflags = get_define("LDFLAGS_" + EXT);
		ldflags = ldflags.replace(/(\.\.\\)/g, rel + "$1");
		contents = contents.replace(/LOCALCPP/, cflags + " /c");
		contents = contents.replace(/LOCALLIBS/, libs);
		contents = contents.replace(/LOCALLDFLAGS/, ldflags);

		if (ext.match("Zend")) {
			arr = new Array("ini", "language");
			parsers = generate_parsers_or_scanners(arr, "Parsers");
			scanners = generate_parsers_or_scanners(arr, "Scanners");
			contents = contents.replace(/DEFS/, parsers + scanners);
		}

		/* none of these are core... */
		contents = contents.replace(/\r\n(CORESOURCES|COREHEADERS|EXTSOURCES|EXTHEADERS|DEFS)\r\n/g, "");

		if (address.match("sapi")) {
			/* most sapis are .dlls, just not cgi, cli, embed */

			if (ext == "cli") {

				/* change of address: php.dsp */
				newext = "cli";
				address = "win32\\php.dsp";
				srcpath = "..\\" + path;
				contents = contents.replace(/cli\.exe/g, "php.exe");

			} else if (ext == "cgi") {

				/* change of address: php-cgi.dsp */
				newext = "cgi";
				address = "win32\\php-cgi.dsp";
				srcpath = "..\\" + path;
				contents = contents.replace(/cgi\.exe/g, "php-cgi.exe");

			} else {

				/* there's always one... most sapis just get a 'php5' prefix */
				newext = (ext.match(/apache2handler/) ? "php5apache2" : "php5" + ext);
				address = address.replace(ext + ".dsp", newext + ".dsp");
				srcpath = ".\\";
				oldext = new RegExp(('[^=\\\\]'+ext), "g");
				contents = contents.replace(oldext, newext);
				contents = contents.replace(ext + ".dll", newext + ".dll");
				contents = contents.replace("CFG=" + ext, "CFG=" + newext);
			}

			contents = read_src_files(ext, contents, (srcpath ? srcpath : false));
			dsp = FSO.CreateTextFile(address, true);
			STDOUT.WriteLine("\tGenerating " + address);
			dsp.Write(contents);
			dsp.Close();

			/* add all configured sapis to the list in php5ts.dsw */
			sblocks += file_get_contents("win32\\build\\block.template.dsw");
			sblocks = sblocks.replace("ADDRESS", address);
			sblocks = sblocks.replace("EXTNAME", newext);

		} else if (address.match("ext") && shared == "true") {

			/* independent modules with their own .dsp */
			contents = read_src_files(ext, contents, false);
			dsp = FSO.CreateTextFile(address, true);
			STDOUT.WriteLine("\tGenerating " + address);
			dsp.Write(contents);
			dsp.Close();

			mblocks += file_get_contents("win32\\build\\block.template.dsw");
			mblocks = mblocks.replace("ADDRESS", address);
			mblocks = mblocks.replace("EXTNAME", ext);

		} else if (ext.match(/(TSRM|Zend)/)) {

			contents = read_src_files(ext, contents, false);
			dsp = FSO.CreateTextFile(address, true);
			STDOUT.WriteLine("\tGenerating " + address);
			dsp.Write(contents);
			dsp.Close();

		} else {

			/* bound for php5[ts].dsp */
			cflags = get_define("CFLAGS_" + EXT);
			cflags = cflags ? cflags.replace(/-(I|D)/g, " /$1") : "";
			cflags = cflags? cflags.replace(/\/(I|D)\s+/g, "/$1") : "";
			cflags = cflags ? cflags.replace(/\/I(?!\")(\S+)/g, '/I"$1"') : "";

			EXT_CFLAGS = check_duplicates(cflags, EXT_CFLAGS);
			EXT_LDFLAGS = check_duplicates(ldflags, EXT_LDFLAGS);
			EXT_LIBS = check_duplicates(libs, EXT_LIBS);

			beginh = '# Begin Group "' + ext + ' "\r\n';
			begins = '# Begin Group "' + ext + '"\r\n';

			hdr = file_get_contents("tmp\\src\\" + ext + ".headers.tmp");
			hdr = hdr.replace(/\.\//g, "..\\" + path);
			hdr = hdr.replace(/\.\.\\\.\.\\/g, "..\\");

			src = file_get_contents("tmp\\src\\" + ext + ".sources.tmp");
			src = src.replace(/\.\//g, "..\\" + path);
			src = src.replace(/\.\.\\\.\.\\/g, "..\\");

			if (ext.match(/(main|standard|streams|win32)/)) {
				CORE_HEADERS += beginh + hdr + "# End Group\r\n";
				CORE_SOURCES += begins + src + "# End Group\r\n";
			} else {
				EXT_HEADERS += beginh + hdr + "# End Group\r\n";
				EXT_SOURCES += begins + src + "# End Group\r\n";
			}

			FSO.DeleteFile("tmp\\src\\" + ext + ".headers.tmp");
			FSO.DeleteFile("tmp\\src\\" + ext + ".sources.tmp");
		}

		FSO.DeleteFile(f.item());
	}

	generate_core_dsp(CORE_HEADERS, CORE_SOURCES, EXT_HEADERS, EXT_SOURCES, EXT_CFLAGS, EXT_LDFLAGS, EXT_LIBS);
	generate_dsw_files(sblocks, mblocks);

	/* goodnight vienna */
	FSO.DeleteFolder("tmp\\src");
	FSO.DeleteFolder("tmp");
}

/* generate source and header entries for .dsp files */
function generate_dsp_filelist(ext, ext_dir, files, intpath)
{
	var EXT = ext.toUpperCase();
	var tabs = new RegExp("[\t\r\n\'\"]", "gm");
	var ws = new RegExp("\\s+", "g");
	var dir = FSO.GetFolder(ext_dir);
	var configfile = FSO.BuildPath(ext_dir, "config.w32");
	var headers = "";
	var path = "";

	if (!files) {
		/* module either lacks a config.w32 or is core
		 * either way, we know nothing about its sources
		 */
		files = "";
		f = new Enumerator(dir.Files);

		for (; !f.atEnd(); f.moveNext()) {
			name = FSO.GetFileName(f.item());

			if (name.substr(name.length-2) == ".c") {
				files += " ./" + name;
			}
		}
	} else {
		files = files.replace(tabs, "");
		files = "./" + files.replace(/ /g, " ./");
	}

	DSP_SOURCES = files.split(" ");

	/* pick up headers (all modules) */
	f = new Enumerator(dir.Files);

	for (; !f.atEnd(); f.moveNext()) {
		name = FSO.GetFileName(f.item());

		if (name.substr(name.length-2) == ".h") {
			headers += " ./" + name;
		}
	}

	DSP_HEADERS = headers.split(" ");

	/* check for bundled library paths and sourcefiles */
	if (FSO.FileExists(configfile)) {
		config = file_get_contents(configfile);

		if (config.match("ADD_SOURCES")) {
			sources = new RegExp("ADD_SOURCES\\([^,]*\\s*,\\s*['\"]([^'\"]+)['\"].*\\)", "gm");
			arr = config.match(sources);
			line = arr[0].replace(tabs, "");
			line = line.replace(/ADD_SOURCES\((.+)\)/, "$1");
			newarr = line.split(',');
			orig_path = newarr[0].replace(/\//g, "\\");
			orig_path = orig_path.replace(/configure_module_dirname(\s?\+\s?)?/, ext_dir);
			path = orig_path.replace(ext_dir + '\\', "");

			if (path.length > 0 && path != ext_dir) {
				subdir = FSO.GetFolder(orig_path);
				lib = new Enumerator(subdir.Files);
				libheaders = "";

				for (; !lib.atEnd(); lib.moveNext()) {
					name = FSO.GetFileName(lib.item());

					if (name.substr(name.length-2) == ".h") {
						libheaders += " ./" + path + "\\" + name;
					}
				}

				DSP_HEADERS = DSP_HEADERS.concat(libheaders.split(" "));

			} else {
				path = "";
			}

			sources = newarr[1].replace(/\\/g, ""); /* continuation lines */
			sources = sources.replace(ws, " ");
			sources = sources.replace(/\s/g, (path ? " ./" + path + "\\" : " ./"));
			sources = check_duplicates(DSP_SOURCES.join(" "), sources);
			DSP_SOURCES = sources.split(" ");
		}
	}

	/* store the array contents in temp files for now */
	write_src_file(ext + ".headers.tmp", ext_dir, intpath, DSP_HEADERS);
	write_src_file(ext + ".sources.tmp", ext_dir, intpath, DSP_SOURCES);

	return;
}

/* entry point. Called from EXTENSION(), SAPI() and generate_files() (confutils.js) */
function generate_dsp_file(ext, ext_dir, files, shared)
{
	var dsp = FSO.CreateTextFile("tmp\\" + ext + ".dsp", true);
	var tmpl = file_get_contents("win32\\build\\template.dsp");
	var ts = (PHP_ZTS != "no" ? "ts" : "");
	var debug = (PHP_DEBUG != "no" ? " /debug" : "");
	var ld = (debug ? "/LDd" : "/LD");
	var status = (PHP_DEBUG == "no" ? 'Release' : 'Debug');
	var statusts = status + (ts ? "_" + ts.toUpperCase() : "");
	var baseflags = "";

	/* store the final path and value of shared in the tmp file */
	if (!ext.match("php5")) {
		tmpl = ext_dir + "\\" + ext + ".dsp#" + shared + tmpl;
	}

	tmpl = tmpl.replace(/extname/g, ext);
	tmpl = tmpl.replace(/Status_TS/g, statusts);

	if (debug) {
		tmpl = tmpl.replace(/Use_Debug_Libraries 0/g, "Use_Debug_Libraries 1");
		tmpl = tmpl.replace(/NDEBUG/g, "_DEBUG");
	}

	if (ext == "cli" || ext == "cgi") {
		tmpl = tmpl.replace(/Dynamic-Link Library/g, "Console Application");
		tmpl = tmpl.replace(/0x0102/, "0x0103");
		path = "..\\";
		type = ".exe";
	} else if (ext == "embed" || ext == "TSRM" || ext == "Zend") {
		tmpl = tmpl.replace(/Dynamic-Link/g, "Static");
		tmpl = tmpl.replace(/0x0102/, "0x0104");
		tmpl = tmpl.replace(/LINK32/g, "LIB32");
		tmpl = tmpl.replace("link.exe", "link.exe -lib");
		tmpl = tmpl.replace(/BASELIBS/g, "/nologo");
		tmpl = tmpl.replace(/\s(LOCALLIBS|BASELDFLAGS|LOCALLDFLAGS|OUTPATH)/g, "");
		path = "..\\";
		if (ext == "embed") {
			path += "..\\";
		}
		type = ".lib";
	} else if (ext.match("php5")) {
		path = "..\\";
		type = ".dll";
	} else {
		path = "..\\..\\";
		type = ".dll";
	}

	outpath = path + get_define("BUILD_DIR");
	tmpl = tmpl.replace(/OUTPUTDIR/g, outpath);

	/* populate the baseline CFLAGS and libs */
	cflags = get_define("CFLAGS").replace(/\s+/g, " ");
	cflags = cflags.replace('/I "..\\bindlib_w32" ', "");
	bcflags = (cflags.replace(/\/([A-Z])\s/g, "/$1")).split(" ");

	for (i= 0; i < bcflags.length; i++) {
		baseflags += (bcflags[i].match(/(PHP|ZEND|ZTS|BASE|FD|WINDOWS)/) ? "" : bcflags[i]);
	}

	baseflags = baseflags.replace(/\//g, " /");
	baseflags = baseflags.substr(1).replace(/(\/D)/g, "$1 ") + " /c";
	tmpl = tmpl.replace(/BASECPP/, (type == ".dll" ? baseflags : baseflags.replace(ld + " ", "")));

	tmpl = tmpl.replace(/BASELIBS/, "/nologo " + get_define("LIBS").replace(/\sresolv.lib/, ""));

	/* now populate the bases in the 'local' lines */
	incs = get_define("BASE_INCLUDES").replace(/\/I (\S+)/g, '/I "' + path + '$1"');
	incs = incs.replace('"' + path + '."', '".."');
	lcflags = cflags.replace(/\$\(BASE_INCLUDES\)/, incs + (type == ".exe" ? '/I "..\\sapi" ' : "") + '/I "' + path + '..\\bindlib_w32"');
	tmpl = tmpl.replace(/BASECPP/, (type == ".dll" ? lcflags : lcflags.replace(ld + " ", "")));
	tmpl = tmpl.replace(/BASELIBS/, "/nologo " + get_define("LIBS") + " " + (ext.match("php5") ? "" : get_define("PHPLIB")));
	ldflags = get_define("LDFLAGS").replace(/\s?(\/nologo|\/libpath:\S+)\s?/g, "");
	tmpl = tmpl.replace(/BASELDFLAGS/, ldflags + (type == ".dll" ? " " + get_define("DLL_LDFLAGS") : "") + (debug ? ' /nodefaultlib:"msvcrt"' : ""));
	out = '/out:"' + outpath + "\\" + ext + type + '"' + ' /libpath:"' + outpath + '"' + ' /libpath:"..\\' + path + 'bindlib_w32\\' + status + '"';
	tmpl = tmpl.replace(/OUTPATH/, out);

	txt = generate_text_filelist(ext, ext_dir);
	res = generate_resource_filelist(ext, ext_dir);

	tmpl = tmpl.replace(/TEXTFILES/, txt);
	tmpl = tmpl.replace(/RESOURCEFILES/, res);

	if (ext.match("php5")) {
		return tmpl;
	}

	/* generate source and header blocks for .dsp */
	generate_dsp_filelist(ext, ext_dir, files, path);

	dsp.Write(tmpl);
	dsp.Close();

	return;
}

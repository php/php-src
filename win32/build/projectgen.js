/* utility function to write .dsp source blocks */
function write_src_file(fname, arr)
{
	var src = FSO.CreateTextFile(fname, true);

	for (i = 0; i < arr.length; i++) {
		if (arr[i].length > 1) {
			src.WriteLine("# Begin Source File");
			src.WriteLine("SOURCE=" + arr[i]);
			src.WriteLine("# End Source File");
			src.WriteBlankLines(1);
		}
	}

	src.Close();
	return;
}

/* final stage, all gets cleaned up here */
function copy_dsp_files()
{
	var tmp = FSO.GetFolder("tmp");
	var core = "TSRM Zend main streams win32 standard";
	var tmpl = file_get_contents("win32\\build\\template.dsw");

	f = new Enumerator(tmp.Files);
	blocks = "";

	for (; !f.atEnd(); f.moveNext()) {
		/* retrieve the final path for the .dsp */
		contents = file_get_contents(f.item());
		address = contents.slice(0, contents.indexOf("#"));
		contents = contents.slice(contents.indexOf("#"));
		dsp = FSO.CreateTextFile(address, true);
		dsp.Write(contents);
		dsp.Close();

		/* pick up module name, check it isn't already in template.dsw */
		ext = address.slice(address.lastIndexOf("\\")+1, address.length-4);

		/* if not, write the workspace block */
		if (!core.match(ext)) {
			blocks += file_get_contents("win32\\build\\block.template.dsw");
			blocks = blocks.replace("ADDRESS", address);
			blocks = blocks.replace("EXTNAME", ext);
		}

		FSO.DeleteFile(f.item());
	}

	/* push all the blocks to the same tag */
	tmpl = tmpl.replace("INSERT", blocks);

	dsw = FSO.CreateTextFile("win32\\php.dsw", true);
	dsw.Write(tmpl);
	dsw.Close();

	/* goodnight vienna */
	FSO.DeleteFolder("tmp");
}

function generate_dsp_flags()
{
	for (i = 0; i < DSP_FLAGS.length; i++) {
		name = DSP_FLAGS[i][0];

		/* it's apparently not possible to key a javascript
		   array using strings, much less dual-key it */
		if (DSP_FLAGS.length == i+1 || name != DSP_FLAGS[i+1][0]) {
			ext = name.substr(7).toLowerCase();
			src = file_get_contents("tmp\\" + ext + ".dsp");
			have = "/D HAVE_" + ext.toUpperCase() + "=1";
			src = src.replace(have, have + " " + DSP_FLAGS[i][1]);
			dsp = FSO.CreateTextFile("tmp\\" + ext + ".dsp", true);
			dsp.Write(src);
			dsp.Close();
		}
	}

	/* now we have flags (and potentially other stuff) for
	   all configured modules, so finish off the process */
	copy_dsp_files();
	return;
}

function generate_dsp_filelist(ext, ext_dir, files)
{
	var EXT = ext.toUpperCase();
	var tabs = new RegExp("[\t\r\n\'\"]", "gm");
	var ws = new RegExp("\\s+", "g");
	var dir = FSO.GetFolder(ext_dir);
	var configfile = FSO.BuildPath(ext_dir, "config.w32");
	var headers = "";

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
			newarr = line.split(',');
			orig_path = newarr[0].replace("ADD_SOURCES(", "");
			munged_dir = ext_dir.replace(/\\/g, '/');
			orig_path = orig_path.replace("configure_module_dirname", munged_dir);
			orig_path = orig_path.replace(" + ", "");
			path = orig_path.replace(munged_dir + '/', "");

			if (path.length > 0 && path != ext) {
				subdir = FSO.GetFolder(orig_path);
				lib = new Enumerator(subdir.Files);
				libheaders = "";

				for (; !lib.atEnd(); lib.moveNext()) {
					name = FSO.GetFileName(lib.item());

					if (name.substr(name.length-2) == ".h") {
						libheaders += " ./" + path + "/" + name;
					}
				}

				DSP_HEADERS = DSP_HEADERS.concat(libheaders.split(" "));

				sources = newarr[1].replace(/\\/g, "");
				sources = sources.replace(ws, " ");
				path = path ? " ./" + path + "/" : " ./";
				sources = sources.replace(/ /g, path);
		
				DSP_SOURCES = DSP_SOURCES.concat(sources.split(" "));
			}
		}
	}

	/* store the array contents in temp files
	   waiting for CFLAGS etc to be populated */
	write_src_file("tmp\\" + ext + ".headers.tmp", DSP_HEADERS);
	write_src_file("tmp\\" + ext + ".sources.tmp", DSP_SOURCES);

	return;
}

/* entry point. Called from EXTENSION(), from SAPI() and (for
   core modules) from generate_files(), all in confutils.js */
function generate_dsp_file(ext, ext_dir, files, shared)
{
	var dsp = FSO.CreateTextFile("tmp\\" + ext + ".dsp", true);
	var tmpl = file_get_contents("win32\\build\\template.dsp");
	var EXT = ext.toUpperCase();

	/* store the final path in the head of the tmp file */
	tmpl = ext_dir + "\\" + ext + ".dsp" + tmpl;

	extname = new RegExp("extname", "gm");
	EXTNAME = new RegExp("EXTNAME", "gm");
	tmpl = tmpl.replace(extname, ext);
	tmpl = tmpl.replace(EXTNAME, EXT);

	status = PHP_DEBUG == "no" ? 'Release' : 'Debug';
	STATUS = new RegExp("Status", "gm");
	tmpl = tmpl.replace(STATUS, status);

	if (PHP_ZTS == "no") {
		zts = new RegExp("_TS", "gmi");
		tmpl = tmpl.replace(zts, '');
	}

	if (PHP_DEBUG != "no") {
		tmpl = tmpl.replace(/Use_Debug_Libraries 0/g, "Use_Debug_Libraries 1");
		tmpl = tmpl.replace(/NDEBUG/g, "_DEBUG");
	}

	cflags = get_define("CFLAGS").replace("$(BASE_INCLUDES)", '/I "..\\.." /I "..\\..\\..\\Zend" /I "..\\..\\TSRM" /I "..\\..\\main" ');
	basecpp = cflags = cflags.replace('/I "..\\bindlib_w32"', '/I "..\\..\\..\\bindlib_w32"');

	if (shared) {
		basecpp += " /D COMPILE_DL_" + EXT;
	}

	tmpl = tmpl.replace("BASECPP", basecpp);
	tmpl = tmpl.replace("BASECPP", cflags + " /D HAVE_" + EXT + "=1");
	tmpl = tmpl.replace(/BASELIBS/g, get_define("LIBS"));
	tmpl = tmpl.replace("LOCALLIBS", get_define("PHPLIB"));

	debug = PHP_DEBUG != "no" ? " /debug" : "";
	dll = shared ? ' /dll /out:"..\\..\\Debug_TS\\php_' + ext + '.dll"' : "";
	tmpl = tmpl.replace(/BASELDFLAGS/g, "/nologo" + debug + dll);

	/* call to generate source and header blocks for .dsp */
	generate_dsp_filelist(ext, ext_dir, files);

	sources = file_get_contents("tmp\\" + ext + ".sources.tmp");
	tmpl = tmpl.replace("SOURCEFILES", sources);
	FSO.DeleteFile("tmp\\" + ext + ".sources.tmp");

	headers = file_get_contents("tmp\\" + ext + ".headers.tmp");
	tmpl = tmpl.replace("HEADERFILES", headers);
	FSO.DeleteFile("tmp\\" + ext + ".headers.tmp");

	dsp.Write(tmpl);
	dsp.Close();

	return;
}

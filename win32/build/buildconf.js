// $Id: buildconf.js,v 1.4 2003-12-04 01:59:46 wez Exp $
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

/* $Id: buildconf.js,v 1.4 2003-12-04 01:59:46 wez Exp $ */
// This generates a configure script for win32 build

WScript.StdOut.WriteLine("Rebuilding configure.js");
var FSO = WScript.CreateObject("Scripting.FileSystemObject");
var C = FSO.CreateTextFile("configure.js", true);

var modules = "";

function file_get_contents(filename)
{
	var F = FSO.OpenTextFile(filename, 1);
	var t = F.ReadAll();
	F.Close();
	return t;
}

function find_config_w32(dirname)
{
	if (!FSO.FolderExists(dirname)) {
		return;
	}

	var f = FSO.GetFolder(dirname);
	var	fc = new Enumerator(f.SubFolders);
	var c;
	for (; !fc.atEnd(); fc.moveNext())
	{
		c = FSO.BuildPath(fc.item(), "config.w32");
		if (FSO.FileExists(c)) {
			//WScript.StdOut.WriteLine(c);
			modules += file_get_contents(c);
		}
	}
}

if (FSO.FileExists("ZendEngine2\\OBJECTS2_HOWTO")) {
	if (FSO.FolderExists("Zend")) {
		FSO.MoveFolder("Zend", "ZendEngine1");
	}
	FSO.MoveFolder("ZendEngine2", "Zend");
}

// Write the head of the configure script
C.WriteLine("/* This file automatically generated from win32/build/confutils.js */");
C.Write(file_get_contents("win32/build/confutils.js"));

// Pull in code from sapi and extensions
modules = file_get_contents("win32/build/config.w32");
find_config_w32("sapi");
find_config_w32("ext");
find_config_w32("pecl");

// Look for ARG_ENABLE or ARG_WITH calls
re = new RegExp("(ARG_(ENABLE|WITH)\([^;]+\);)", "gm");
calls = modules.match(re);
for (i = 0; i < calls.length; i++) {
	item = calls[i];
	C.WriteLine(item);
}

C.WriteBlankLines(1);
C.WriteLine("conf_process_args();");
C.WriteBlankLines(1);

// Comment out the calls from their original positions
modules = modules.replace(re, "/* $1 */");
C.Write(modules);

C.WriteBlankLines(1);
C.Write(file_get_contents("win32/build/configure.tail"));

WScript.StdOut.WriteLine("Now run 'cscript /nologo configure.js --help'");


/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2009 The PHP Group                                |
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
  |         Pierre A. Joye <pierre@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
// Cleans up files that do not belong in the repository

var FSO = WScript.CreateObject("Scripting.FileSystemObject");
var WshShell = WScript.CreateObject("WScript.Shell");
var STDOUT = WScript.StdOut;

/* svn propget svn:ignore dirname */
function find_ignore(dirname)
{
	dirname = "" + dirname;
	dirname_len = dirname.length;

	if (!FSO.FolderExists(dirname) || (dirname_len >= 4 && 
	dirname.substring(dirname_len - 4) == ".svn")) {
		return;
	}

	var f = FSO.GetFolder(dirname);
	var fc = new Enumerator(f.SubFolders);

	for (; !fc.atEnd(); fc.moveNext()) {
		find_ignore(fc.item());
	}

	kill_from_ignore(dirname);
}

/* recursive remove using ignore props style wildcard matching;
 * note that FSO.DeleteFolder and FSO.DeleteFile methods both
 * accept wildcards, but that they are dangerous to use eg:
 * "*.php" will match "*.phpt" */
function rm_r(filename)
{
	if (FSO.FolderExists(filename)) {
		var fc = new Enumerator(FSO.GetFolder(filename).SubFolders);

		for (; !fc.atEnd(); fc.moveNext()) {
			rm_r(fc.item());
		}

		fc = new Enumerator(FSO.GetFolder(filename).Files);

		for (; !fc.atEnd(); fc.moveNext()) {
			FSO.DeleteFile(fc.item(), true);
		}

		FSO.DeleteFolder(filename, true);
	} else if (FSO.FileExists(filename)) {
		FSO.DeleteFile(filename, true);
	} else {
		/* we need to handle wildcards here */
		var foldername = FSO.GetParentFolderName(filename);

		if (foldername == "")
			foldername = ".";

		var filename = FSO.GetFileName(filename);

		var retext = filename.replace(/\./g, '\\.');
		retext = '^' + retext.replace(/\*/g, '.*') + "$";
		var re = new RegExp(retext);

		var folder = FSO.GetFolder(foldername);
		var fc = new Enumerator(folder.SubFolders);
		for (; !fc.atEnd(); fc.moveNext()) {

			var item = FSO.GetFileName(fc.item());

			if (item.match(re)) {
				rm_r(fc.item());
			}
		}
		var fc = new Enumerator(folder.Files);
		for (; !fc.atEnd(); fc.moveNext()) {
			item = FSO.GetFileName(fc.item());

			if (item.match(re)) {
				FSO.DeleteFile(fc.item(), true);
			}
		}
	}
}

function kill_from_ignore(dirname)
{
	var l;
	var e = WshShell.Exec("svn propget svn:ignore " + dirname);
	var re = /^(config\.nice.*)|(\*)$/i;

	while (!e.StdOut.atEndOfStream) {
		l = e.StdOut.ReadLine();
		if (l.length == 0 || re.test(l)) {
			continue;
		}
		rm_r(dirname + l);
	}

}

find_ignore(".");

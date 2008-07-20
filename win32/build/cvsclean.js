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

/* $Id: cvsclean.js,v 1.5.2.1.2.1.2.1 2008-07-20 02:20:31 sfox Exp $ */
// Cleans up files that do not belong in CVS

var FSO = WScript.CreateObject("Scripting.FileSystemObject");

function find_cvsignore(dirname)
{
	if (!FSO.FolderExists(dirname))
		return;

	var f = FSO.GetFolder(dirname);
	var fc = new Enumerator(f.SubFolders);
	
	for (; !fc.atEnd(); fc.moveNext()) {
		find_cvsignore(fc.item());	
	}

	if (FSO.FileExists(dirname + "\\.cvsignore")) {
		kill_from_cvsignore(dirname + "\\.cvsignore");
	}
}

/* recursive remove using cvsignore style wildcard matching;
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

function kill_from_cvsignore(igfile)
{
	var dir = FSO.GetParentFolderName(igfile) + "\\";
	var t = FSO.OpenTextFile(igfile, 1);
	var l;
	
	if (dir == ".\\") {
		dir = "";
	}
		
	while (!t.atEndOfStream) {
		l = t.ReadLine();
		// don't kill their config.nice file(s)
		if (l.match("config\.nice.*") ||
			l.match("") || 
			l.match("*"))
			continue;
		rm_r(dir + l);
	}

}

find_cvsignore(".");


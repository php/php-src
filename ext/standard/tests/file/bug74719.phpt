--TEST--
Bug #74719 Allow NULL as context
--CREDITS--
Alexander Holman <alexander@holman.org.uk>
--FILE--
<?php
/* Prototype  : resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
 * Description: Open a file or a URL and return a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

require_once('fopen_include_path.inc');

$thisTestDir =  basename(__FILE__, ".php") . ".dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$newpath = relative_include_path();
set_include_path($newpath);

$tmpfile =  basename(__FILE__, ".php") . ".tmp";
$h = fopen($tmpfile, "w", true, NULL);
if ($h === false) {
    echo "Unable to open file to write\n";
}
else {
    echo "open and write okay\n";
    fwrite($h, "This is the test file");
	fclose($h);
	$h = fopen($tmpfile, "r", false, NULL);
	if ($h === false) {
	   echo "Unable to open file to read file\n";
	}
	else {
	   echo "open and read okay\n";
	   fclose($h);
	}
	unlink($tmpfile);
}

teardown_relative_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);

function runtest() {
}
?>
===DONE===
--EXPECT--
open and write okay
open and read okay
===DONE===

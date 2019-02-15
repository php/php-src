--TEST--
Test file_get_contents() function : variation - include path testing
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : string file_get_contents(string filename [, bool use_include_path [, resource context [, long offset [, long maxlen]]]])
 * Description: Read the entire file into a string
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing file_get_contents() : variation ***\n";


require_once('fopen_include_path.inc');

// this doesn't create the include dirs in this directory
// we change to this to ensure we are not part of the
// include paths.
$thisTestDir = "FileGetContentsVar2.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = 'FileGetContentsVar2.tmp';
$scriptLocFile = dirname(__FILE__)."/".$filename;

$newpath = create_include_path();
set_include_path($newpath);
runtest();
teardown_include_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $scriptLocFile, $filename;
   $h = fopen($scriptLocFile, "w");
   fwrite($h, "File in script location");
   fclose($h);
   $line = file_get_contents($filename, true);
   echo "$line\n";
   unlink($scriptLocFile);
}

?>
===DONE===
--EXPECT--
*** Testing file_get_contents() : variation ***
File in script location
===DONE===

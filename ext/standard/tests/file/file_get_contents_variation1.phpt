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
$thisTestDir = "fileGetContentsVar1.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = "afile.txt";
$secondFile = $dir2."/".$filename;

$newpath = create_include_path();
set_include_path($newpath);
runtest();
teardown_include_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $secondFile, $filename;
   $h = fopen($secondFile, "w");
   fwrite($h, "File in include path");
   fclose($h);
   $line = file_get_contents($filename, true);
   echo "$line\n";
   unlink($secondFile);  
}

?>
===DONE===
--EXPECT--
*** Testing file_get_contents() : variation ***
File in include path
===DONE===

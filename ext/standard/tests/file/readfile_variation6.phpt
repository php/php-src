--TEST--
Test readfile() function : variation - test include path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : int readfile(string filename [, bool use_include_path[, resource context]])
 * Description: Output a file or a URL 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

require_once('fopen_include_path.inc');

echo "*** Testing readfile() : variation ***\n";
// this doesn't create the include dirs in this directory
// we change to this to ensure we are not part of the
// include paths.
$thisTestDir = "readfileVar6.dir";
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
   readfile($filename, true);
   echo "\n";
   unlink($secondFile);  
}

?>
===DONE===
--EXPECT--
*** Testing readfile() : variation ***
File in include path
===DONE===

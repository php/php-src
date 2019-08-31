--TEST--
Test readfile() function : variation
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
$thisTestDir = "readfileVar7.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = "readFileVar7.tmp";
$scriptLocFile = __DIR__."/".$filename;

$newpath = create_include_path();
set_include_path($newpath);
runtest();
teardown_include_path();
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $scriptLocFile, $filename;
   $h = fopen($scriptLocFile, "w");
   fwrite($h, "File in script location");
   fclose($h);
   readfile($filename, true);
   echo "\n";
   unlink($scriptLocFile);
}

?>
===DONE===
--EXPECT--
*** Testing readfile() : variation ***
File in script location
===DONE===

--TEST--
Test file_put_contents() function : variation - include path testing
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : int file_put_contents(string file, mixed data [, int flags [, resource context]])
 * Description: Write/Create a file with contents data and return the number of bytes written 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing file_put_contents() : variation ***\n";

require_once('fopen_include_path.inc');

// this doesn't create the include dirs in this directory
// we change to this to ensure we are not part of the
// include paths.
$thisTestDir = "fileGetContentsVar7.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = "readFileVar7.tmp";
$scriptLocFile = dirname(__FILE__)."/".$filename;

$newpath = "rubbish";
set_include_path($newpath);
runtest();
$newpath = "";
set_include_path($newpath);
runtest();
set_include_path(null);
runtest();
set_include_path(";;  ; ;c:\\rubbish");
runtest();
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $scriptLocFile, $filename;
   file_put_contents($filename, "File in script location", FILE_USE_INCLUDE_PATH);
   $line = file_get_contents($scriptLocFile);
   echo "$line\n";
   unlink($scriptLocFile);     
}

?>
===DONE===
--EXPECT--
*** Testing file_put_contents() : variation ***
File in script location
File in script location
File in script location
File in script location
===DONE===


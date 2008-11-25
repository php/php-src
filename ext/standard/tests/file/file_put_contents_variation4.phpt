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
$thisTestDir = "filePutContentsVar4.dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = "afile.txt";
$firstFile = $dir1."/".$filename;

$newpath = create_include_path();
set_include_path($newpath);
runtest();
$newpath = generate_next_path();
set_include_path($newpath);
runtest();
teardown_include_path();
restore_include_path();
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $firstFile, $filename;
   file_put_contents($filename, "File in include path", FILE_USE_INCLUDE_PATH);
   $line = file_get_contents($firstFile);
   echo "$line\n";
   unlink($firstFile);  
}

?>
===DONE===
--EXPECT--
*** Testing file_put_contents() : variation ***
File in include path
File in include path
===DONE===

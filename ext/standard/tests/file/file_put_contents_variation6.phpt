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

$thisTestDir = basename(__FILE__, ".php") . ".dir";
mkdir($thisTestDir);
chdir($thisTestDir);

$filename = basename(__FILE__, ".php") . ".tmp";

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
   global $filename;

   //correct php53 behaviour is to ignore the FILE_USE_INCLUDE_PATH unless the file already exists
   // in the include path. In this case it doesn't so the file should be written in the current dir.

   file_put_contents($filename, "File in include path", FILE_USE_INCLUDE_PATH);
   file_put_contents($filename, ". This was appended", FILE_USE_INCLUDE_PATH | FILE_APPEND);
   $line = file_get_contents($filename);
   echo "$line\n";
   unlink($filename);
}

?>
===DONE===
--EXPECT--
*** Testing file_put_contents() : variation ***
File in include path. This was appended
File in include path. This was appended
===DONE===

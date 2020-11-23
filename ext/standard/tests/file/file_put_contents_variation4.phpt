--TEST--
Test file_put_contents() function : variation - include path testing
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
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
chdir("..");
rmdir($thisTestDir);


function runtest() {
   global $filename;
   //correct php53 behaviour is to ignore the FILE_USE_INCLUDE_PATH unless the file already exists
   // in the include path. In this case it doesn't so the file should be written in the current dir.
   file_put_contents($filename, "File in include path", FILE_USE_INCLUDE_PATH);
   $line = file_get_contents($filename);
   echo "$line\n";
   unlink($filename);
}

?>
--EXPECT--
File in include path
File in include path

--TEST--
Test unlink() function : variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing unlink() : variation: contexts and relative files ***\n";

// test relative directories and stream contexts.
$subdir = 'subdir';
$testfile = $subdir.'/testfile.txt';
mkdir($subdir);
touch($testfile);
f_exists($testfile);
$context = stream_context_create();
var_dump(unlink($testfile, $context));
f_exists($testfile);
rmdir($subdir);

function f_exists($file) {
   if (file_exists($file) == true) {
      echo "$file exists\n";
   }
   else {
      echo "$file doesn't exist\n";
   }
}
?>
--EXPECT--
*** Testing unlink() : variation: contexts and relative files ***
subdir/testfile.txt exists
bool(true)
subdir/testfile.txt doesn't exist

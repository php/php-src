--TEST--
Test file() function : error conditions
--FILE--
<?php
$file_path = __DIR__;
echo "\n*** Testing error conditions ***\n";
$file_handle = fopen($file_path."/file.tmp", "w");

$filename = $file_path."/file.tmp";
var_dump( file($filename, 10, NULL) );  //  Incorrect flag

var_dump( file("temp.tmp") );  // non existing filename
fclose($file_handle);

echo "\n--- Done ---";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/file.tmp");
?>
--EXPECTF--
*** Testing error conditions ***
array(0) {
}

Warning: file(temp.tmp): Failed to open stream: No such file or directory in %s on line %d
bool(false)

--- Done ---

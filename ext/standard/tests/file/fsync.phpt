--TEST--
Test fsync() function: basic functionality
--FILE--
<?php

echo "*** Testing fsync(): writing to a file and reading the contents ***\n";
$data = <<<EOD
first line of string
second line of string
third line of string
EOD;

$file_path = __DIR__;
$filename = "$file_path/fsync_basic.tmp";

// opening a file
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

if(PHP_OS_FAMILY == 'Windows')  {
    $data = str_replace("\r",'', $data);
}

// writing data to the file
var_dump( fwrite($file_handle, $data) );
var_dump( fsync($file_handle) );
var_dump( readfile($filename) );

echo "\n*** Testing fsync(): for return type ***\n";
$return_value = fsync($file_handle);
var_dump( is_bool($return_value) );
fclose($file_handle);

echo "\n*** Testing fsync(): attempting to sync stdin ***\n";
$file_handle = fopen("php://stdin", "w");
var_dump(fsync($file_handle));
fclose($file_handle);

echo "\n*** Testing fsync(): for non-file stream ***\n";
$file_handle = fopen("php://memory", "w");
$return_value = fsync($file_handle);
var_dump( ($return_value) );
fclose($file_handle);

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fsync_basic.tmp";
unlink($filename);
?>
--EXPECTF--
*** Testing fsync(): writing to a file and reading the contents ***
int(63)
bool(true)
first line of string
second line of string
third line of stringint(63)

*** Testing fsync(): for return type ***
bool(true)

*** Testing fsync(): attempting to sync stdin ***
bool(false)

*** Testing fsync(): for non-file stream ***

Warning: fsync(): Can't fsync this stream! in %s on line %d
bool(false)

*** Done ***

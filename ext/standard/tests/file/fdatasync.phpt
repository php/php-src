--TEST--
Test fdatasync() function: basic functionality
--FILE--
<?php

echo "*** Testing fdatasync(): writing to a file and reading the contents ***\n";
$data = <<<EOD
first line of string
second line of string
third line of string
EOD;

$file_path = __DIR__;
$filename = "$file_path/fdatasync_basic.tmp";

// opening a file
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

if(PHP_OS_FAMILY == 'Windows')  {
    $data = str_replace("\r",'', $data);
}

// writing data to the file
var_dump( fwrite($file_handle, $data) );
var_dump( fdatasync($file_handle) );
var_dump( readfile($filename) );

echo "\n*** Testing fdatasync(): for return type ***\n";
$return_value = fdatasync($file_handle);
var_dump( is_bool($return_value) );
fclose($file_handle);

echo "\n*** Testing fdatasync(): attempting to sync stdin ***\n";
$file_handle = fopen("php://stdin", "w");
var_dump(fdatasync($file_handle));
fclose($file_handle);

echo "\n*** Testing fdatasync(): for non-file stream ***\n";
$file_handle = fopen("php://memory", "w");
$return_value = fdatasync($file_handle);
var_dump( ($return_value) );
fclose($file_handle);

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fdatasync_basic.tmp";
unlink($filename);
?>
--EXPECTF--
*** Testing fdatasync(): writing to a file and reading the contents ***
int(63)
bool(true)
first line of string
second line of string
third line of stringint(63)

*** Testing fdatasync(): for return type ***
bool(true)

*** Testing fdatasync(): attempting to sync stdin ***
bool(false)

*** Testing fdatasync(): for non-file stream ***

Warning: fdatasync(): Can't fsync this stream! in %s on line %d
bool(false)

*** Done ***

--TEST--
Test fflush() function: basic functionality
--FILE--
<?php
/*  Prototype: bool fflush ( resource $handle );
    Description: Flushes the output to a file
*/

echo "*** Testing fflush(): writing to a file and reading the contents ***\n";
$data = <<<EOD
first line of string
second line of string
third line of string
EOD;

$file_path = dirname(__FILE__);
$filename = "$file_path/fflush_basic.tmp";

// opening a file
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

if(substr(PHP_OS, 0, 3) == "WIN")  {
	$data = str_replace("\r",'', $data);
}

// writing data to the file
var_dump( fwrite($file_handle, $data) );
var_dump( fflush($file_handle) );
var_dump( readfile($filename) );

echo "\n*** Testing fflush(): for return type ***\n";
$return_value = fflush($file_handle);
var_dump( is_bool($return_value) );
fclose($file_handle);
echo "\n*** Done ***";
?>

--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fflush_basic.tmp";
unlink($filename);
?>

--EXPECTF--
*** Testing fflush(): writing to a file and reading the contents ***
int(63)
bool(true)
first line of string
second line of string
third line of stringint(63)

*** Testing fflush(): for return type ***
bool(true)

*** Done ***

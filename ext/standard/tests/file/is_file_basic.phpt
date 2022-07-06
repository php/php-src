--TEST--
Test is_file() function: basic functionality
--FILE--
<?php
echo "*** Testing is_file(): basic functionality ***\n";

/* Checking with current file */
var_dump( is_file(__FILE__) );

/* Checking with (current) dir */
var_dump( is_file(__DIR__) );

$file_path = __DIR__;
$file_name = $file_path."/is_file_basic.tmp";
/* With non-existing file */
var_dump( is_file($file_name) );
/* With existing file */
fclose( fopen($file_name, "w") );
var_dump( is_file($file_name) );

echo "*** Testing is_file() for its return value type ***\n";
var_dump( is_bool( is_file(__FILE__) ) );
var_dump( is_bool( is_file("/no/such/file") ) );

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$file_name = $file_path."/is_file_basic.tmp";
unlink($file_name);
?>
--EXPECT--
*** Testing is_file(): basic functionality ***
bool(true)
bool(false)
bool(false)
bool(true)
*** Testing is_file() for its return value type ***
bool(true)
bool(true)

*** Done ***

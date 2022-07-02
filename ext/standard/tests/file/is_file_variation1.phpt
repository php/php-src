--TEST--
Test is_file() function: usage variations - diff. files
--FILE--
<?php
/* Testing is_file() with file containing data, truncating its size
     and the file created by touch() */

$file_path = __DIR__;

echo "-- Testing is_file() with file containing data --\n";
$filename = $file_path."/is_file_variation1.tmp";
$file_handle = fopen($filename, "w" );
fwrite( $file_handle, "Hello, world....." ); // expected true
fclose($file_handle);
var_dump( is_file($filename) );
clearstatcache();

echo "\n-- Testing is_file() after truncating filesize to zero bytes --\n";
$file_handle = fopen( $file_path."/is_file_variation1.tmp", "r");
ftruncate($file_handle, 0);
fclose($file_handle);
var_dump( is_file($file_path."/is_file_variation1.tmp") ); // expected true
clearstatcache();
unlink($file_path."/is_file_variation1.tmp");

echo "\n-- Testing is_file() with an empty file --\n";
/* created by fopen() */
fclose( fopen($file_path."/is_file_variation1.tmp", "w") );
var_dump( is_file($file_path."/is_file_variation1.tmp") );  //expected true
clearstatcache();
unlink($file_path."/is_file_variation1.tmp");

/* created by touch() */
touch($file_path."/is_file_variation1.tmp");
var_dump( is_file($file_path."/is_file_variation1.tmp") ); // expected true
clearstatcache();
unlink($file_path."/is_file_variation1.tmp");

echo "\n*** Done ***";
?>
--EXPECT--
-- Testing is_file() with file containing data --
bool(true)

-- Testing is_file() after truncating filesize to zero bytes --
bool(true)

-- Testing is_file() with an empty file --
bool(true)
bool(true)

*** Done ***

--TEST--
Test file_put_contents() and file_get_contents() functions : basic functionality
--FILE--
<?php

/* Prototype: string file_get_contents( string $filename[, bool $use_include_path[, 
 *                                      resource $context[, int $offset[, int $maxlen]]]] )
 * Description: Reads entire file into a string
 */

/* Prototype: int file_put_contents( string $filename, mixed $data[, int $flags[, resource $context]] )
 * Description: Write a string to a file
 */

$file_path = dirname(__FILE__);
include($file_path."/file.inc");
$filename = "/file_get_contents.tmp";

echo "*** Testing the basic functionality ***\n";

echo "-- Testing with simple valid data file --\n";

fill_buffer($text_buffer, "text", 100);
file_put_contents( $file_path.$filename, (binary)$text_buffer );

var_dump( file_get_contents($file_path.$filename) );

echo "\n-- Testing with empty file --\n";

$filename = "/file_get_contents1.tmp";
file_put_contents( $file_path.$filename, "");
var_dump( file_get_contents( $file_path.$filename ) );

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/file_get_contents.tmp");
unlink($file_path."/file_get_contents1.tmp");
?>
--EXPECT--
*** Testing the basic functionality ***
-- Testing with simple valid data file --
string(100) "text text text text text text text text text text text text text text text text text text text text "

-- Testing with empty file --
string(0) ""

*** Done ***

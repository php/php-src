--TEST--
Test file_get_contents() and file_put_contents() functions : usage variations - all arguments
--FILE--
<?php
/* Prototype: string file_get_contents( string $filename[, bool $use_include_path[,
 *                                      resource $context[, int $offset[, int $maxlen]]]] )
 * Description: Reads entire file into a string
 */

/* Prototype: int file_put_contents( string $filename, mixed $data[,int $flags[, resource $context]] )
 * Description: Write a string to a file
 */

/* Testing variation in all argument values */
$file_path = __DIR__;
include($file_path."/file.inc");

echo "*** Testing with variations in the arguments values ***\n";

$buffer_types = array("text", "numeric", "text_with_new_line", "alphanumeric");

foreach( $buffer_types as $type) {
  fill_buffer($buffer, $type, 100);
  file_put_contents( $file_path."/file_put_contents_variation1.tmp", $buffer);
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 0) );
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 1) );
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 0, NULL, 5) );
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 1, NULL, 5) );
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 0, NULL, 5, 20) );
  var_dump( file_get_contents($file_path."/file_put_contents_variation1.tmp", 1, NULL, 5, 20) );

}

echo "--- Done ---";
?>
--CLEAN--
<?php
//Deleting the temporary file

$file_path = __DIR__;
unlink($file_path."/file_put_contents_variation1.tmp");
?>
--EXPECT--
*** Testing with variations in the arguments values ***
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(100) "text text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(95) "text text text text text text text text text text text text text text text text text text text "
string(20) "text text text text "
string(20) "text text text text "
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(95) "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
string(20) "22222222222222222222"
string(20) "22222222222222222222"
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(100) "line
line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(95) "line of text
line
line of text
line
line of text
line
line of text
line
line of text
line
line "
string(20) "line of text
line
li"
string(20) "line of text
line
li"
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(95) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
string(20) "ab12 ab12 ab12 ab12 "
--- Done ---

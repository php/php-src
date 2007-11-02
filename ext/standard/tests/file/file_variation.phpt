--TEST--
Test file() function : usage variations
--FILE--
<?php
/* 
 * Prototype: array file ( string $filename [,int $flags [,resource $context]] );
 * Description: Reads entire file into an array
 *              Returns the  file in an array
 */

$file_path = dirname(__FILE__);
require($file_path."/file.inc");
$filename = $file_path."/file_variation.tmp";

$data_array = array( "Garbage data", "Gar\nba\nge d\nata", "Gar\n\nbage \n\n data" );

echo "*** Using various flags values with different data in a file\n";
$count=1;
foreach( $data_array as $data ) {
  echo "--Iteration $count --\n";
  $fh  = fopen($filename, "w");
  fwrite($fh, $data);
  var_dump( file($filename, FILE_IGNORE_NEW_LINES) ); 
  var_dump( file($filename, FILE_SKIP_EMPTY_LINES) ); 
  $count++;
  fclose($fh);
}

echo "*** Testing with variation in use_include_path argument ***\n";
$dir = "/file_variation";
$file_path = dirname(__FILE__).$dir;

mkdir($file_path);
ini_set( 'include_path', $file_path );
$filename = $file_path."/file1_variation.tmp";

$buffer_types = array("text", "numeric", "alphanumeric", "text_with_new_line");
foreach( $buffer_types as $type) {
  fill_buffer($buffer, $type, 100);
  file_put_contents($filename, $buffer );
  var_dump( file("file1_variation.tmp", FILE_USE_INCLUDE_PATH) );
  var_dump( file($filename, FILE_IGNORE_NEW_LINES) );
  var_dump( file($filename, FILE_SKIP_EMPTY_LINES) );
   
}

echo "\n--- Done ---";
?>

--CLEAN--
<?php
// Removing the temporary files and directory

$file_path = dirname(__FILE__);
unlink($file_path."/file_variation.tmp");

$dir = "/file_variation";
$file_path = dirname(__FILE__).$dir;
unlink($file_path."/file1_variation.tmp");
rmdir($file_path);

?>
--EXPECTF--
*** Using various flags values with different data in a file
--Iteration 1 --
array(1) {
  [0]=>
  string(12) "Garbage data"
}
array(1) {
  [0]=>
  string(12) "Garbage data"
}
--Iteration 2 --
array(4) {
  [0]=>
  string(3) "Gar"
  [1]=>
  string(2) "ba"
  [2]=>
  string(4) "ge d"
  [3]=>
  string(3) "ata"
}
array(4) {
  [0]=>
  string(4) "Gar
"
  [1]=>
  string(3) "ba
"
  [2]=>
  string(5) "ge d
"
  [3]=>
  string(3) "ata"
}
--Iteration 3 --
array(5) {
  [0]=>
  string(3) "Gar"
  [1]=>
  string(0) ""
  [2]=>
  string(5) "bage "
  [3]=>
  string(0) ""
  [4]=>
  string(5) " data"
}
array(5) {
  [0]=>
  string(4) "Gar
"
  [1]=>
  string(1) "
"
  [2]=>
  string(6) "bage 
"
  [3]=>
  string(1) "
"
  [4]=>
  string(5) " data"
}
*** Testing with variation in use_include_path argument ***
array(1) {
  [0]=>
  string(100) "text text text text text text text text text text text text text text text text text text text text "
}
array(1) {
  [0]=>
  string(100) "text text text text text text text text text text text text text text text text text text text text "
}
array(1) {
  [0]=>
  string(100) "text text text text text text text text text text text text text text text text text text text text "
}
array(1) {
  [0]=>
  string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
}
array(1) {
  [0]=>
  string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
}
array(1) {
  [0]=>
  string(100) "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
}
array(1) {
  [0]=>
  string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
}
array(1) {
  [0]=>
  string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
}
array(1) {
  [0]=>
  string(100) "ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 ab12 "
}
array(12) {
  [0]=>
  string(5) "line
"
  [1]=>
  string(13) "line of text
"
  [2]=>
  string(5) "line
"
  [3]=>
  string(13) "line of text
"
  [4]=>
  string(5) "line
"
  [5]=>
  string(13) "line of text
"
  [6]=>
  string(5) "line
"
  [7]=>
  string(13) "line of text
"
  [8]=>
  string(5) "line
"
  [9]=>
  string(13) "line of text
"
  [10]=>
  string(5) "line
"
  [11]=>
  string(5) "line "
}
array(12) {
  [0]=>
  string(4) "line"
  [1]=>
  string(12) "line of text"
  [2]=>
  string(4) "line"
  [3]=>
  string(12) "line of text"
  [4]=>
  string(4) "line"
  [5]=>
  string(12) "line of text"
  [6]=>
  string(4) "line"
  [7]=>
  string(12) "line of text"
  [8]=>
  string(4) "line"
  [9]=>
  string(12) "line of text"
  [10]=>
  string(4) "line"
  [11]=>
  string(5) "line "
}
array(12) {
  [0]=>
  string(5) "line
"
  [1]=>
  string(13) "line of text
"
  [2]=>
  string(5) "line
"
  [3]=>
  string(13) "line of text
"
  [4]=>
  string(5) "line
"
  [5]=>
  string(13) "line of text
"
  [6]=>
  string(5) "line
"
  [7]=>
  string(13) "line of text
"
  [8]=>
  string(5) "line
"
  [9]=>
  string(13) "line of text
"
  [10]=>
  string(5) "line
"
  [11]=>
  string(5) "line "
}

--- Done ---

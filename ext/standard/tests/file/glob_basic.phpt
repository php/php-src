--TEST--
Test glob() function: basic functions
--FILE--
<?php
/* Prototype: array glob ( string $pattern [, int $flags] );
   Description: Find pathnames matching a pattern
*/

echo "*** Testing glob() : basic functions ***\n";

$file_path = dirname(__FILE__);

// temp dirname used here
$dirname = "$file_path/glob_basic";

// temp dir created
mkdir($dirname);

// temp files created
$fp = fopen("$dirname/wonder12345", "w");
fclose($fp);
$fp = fopen("$dirname/wonder.txt", "w");
fclose($fp);
$fp = fopen("$dirname/file.text", "w");
fclose($fp);

// glob() with default arguments
var_dump( glob($dirname."/*") );
var_dump( glob($dirname."/*.txt") );
var_dump( glob($dirname."/*.t?t") );
var_dump( glob($dirname."/*.t*t") );
var_dump( glob($dirname."/*.?") );
var_dump( glob($dirname."/*.*") );

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink("$file_path/glob_basic/wonder12345");
unlink("$file_path/glob_basic/wonder.txt");
unlink("$file_path/glob_basic/file.text");
rmdir("$file_path/glob_basic/");
?>
--EXPECTF--
*** Testing glob() : basic functions ***
array(3) {
  [0]=>
  string(%d) "%s/glob_basic/file.text"
  [1]=>
  string(%d) "%s/glob_basic/wonder.txt"
  [2]=>
  string(%d) "%s/glob_basic/wonder12345"
}
array(1) {
  [0]=>
  string(%d) "%s/glob_basic/wonder.txt"
}
array(1) {
  [0]=>
  string(%d) "%s/glob_basic/wonder.txt"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_basic/file.text"
  [1]=>
  string(%d) "%s/glob_basic/wonder.txt"
}
array(0) {
}
array(2) {
  [0]=>
  string(%d) "%s/glob_basic/file.text"
  [1]=>
  string(%d) "%s/glob_basic/wonder.txt"
}
Done

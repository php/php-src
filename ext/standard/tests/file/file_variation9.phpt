--TEST--
Test file function : variation - test various endings of a file
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : array file(string filename [, int flags[, resource context]])
 * Description: Read entire file into an array
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing file() : variation ***\n";
$testfile = dirname(__FILE__)."/fileVar9.txt";

$contents = array(
   "File ends on a single character\na",
   "File ends on a new line\n",
   "File ends on multiple newlines\n\n\n\n",
   "File has\n\nmultiple lines and newlines\n\n",
   "File has\r\nmultiple crlfs\n\r\n"
   );

@unlink($testfile);
foreach ($contents as $content) {
    $h = fopen($testfile, "w");
    fwrite($h, $content);
    fclose($h);
    var_dump(file($testfile));
	unlink($testfile);
}

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing file() : variation ***
array(2) {
  [0]=>
  string(32) "File ends on a single character
"
  [1]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(24) "File ends on a new line
"
}
array(4) {
  [0]=>
  string(31) "File ends on multiple newlines
"
  [1]=>
  string(1) "
"
  [2]=>
  string(1) "
"
  [3]=>
  string(1) "
"
}
array(4) {
  [0]=>
  string(9) "File has
"
  [1]=>
  string(1) "
"
  [2]=>
  string(28) "multiple lines and newlines
"
  [3]=>
  string(1) "
"
}
array(3) {
  [0]=>
  string(10) "File has
"
  [1]=>
  string(15) "multiple crlfs
"
  [2]=>
  string(2) "
"
}

*** Done ***

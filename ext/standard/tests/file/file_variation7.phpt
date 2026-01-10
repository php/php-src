--TEST--
file() on a file with blank lines
--WHITESPACE_SENSITIVE--
--FILE--
<?php

$filepath = __DIR__ . '/file_variation_7.tmp';
$fd = fopen($filepath, "w+");
fwrite($fd, "Line 1\n\n \n  \n\Line 3");
fclose($fd);

echo "file():\n";
var_dump(file($filepath));

echo "\nfile() with FILE_IGNORE_NEW_LINES:\n";
var_dump(file($filepath, FILE_IGNORE_NEW_LINES));

echo "\nfile() with FILE_SKIP_EMPTY_LINES:\n";
var_dump(file($filepath, FILE_SKIP_EMPTY_LINES));

echo "\nfile() with FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES:\n";
var_dump(file($filepath, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES));


?>
--CLEAN--
<?php
$filepath = __DIR__ . '/file_variation_7.tmp';
unlink($filepath);
?>
--EXPECT--
file():
array(5) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(1) "
"
  [2]=>
  string(2) " 
"
  [3]=>
  string(3) "  
"
  [4]=>
  string(7) "\Line 3"
}

file() with FILE_IGNORE_NEW_LINES:
array(5) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) " "
  [3]=>
  string(2) "  "
  [4]=>
  string(7) "\Line 3"
}

file() with FILE_SKIP_EMPTY_LINES:
array(5) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(1) "
"
  [2]=>
  string(2) " 
"
  [3]=>
  string(3) "  
"
  [4]=>
  string(7) "\Line 3"
}

file() with FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES:
array(4) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(1) " "
  [2]=>
  string(2) "  "
  [3]=>
  string(7) "\Line 3"
}

--TEST--
Test file() function with FILE_SKIP_EMPTY_LINES flag
--FILE--
<?php
$test_file = __DIR__ . "/file_skip_empty_lines.tmp";

$test_data = "First\n\nSecond\n\n\nThird\n";
file_put_contents($test_file, $test_data);

$lines = file($test_file, FILE_SKIP_EMPTY_LINES);
var_dump($lines);

$lines = file($test_file, FILE_SKIP_EMPTY_LINES | FILE_IGNORE_NEW_LINES);
var_dump($lines);

$lines = file($test_file, FILE_IGNORE_NEW_LINES);
var_dump($lines);

$lines = file($test_file);
var_dump($lines);

$test_data_win = "First\r\n\r\nSecond\r\n";
file_put_contents($test_file, $test_data_win);

$lines = file($test_file, FILE_SKIP_EMPTY_LINES);
var_dump($lines);

$lines = file($test_file, FILE_SKIP_EMPTY_LINES | FILE_IGNORE_NEW_LINES);
var_dump($lines);

unlink($test_file);
?>
--EXPECT--
array(3) {
  [0]=>
  string(6) "First
"
  [1]=>
  string(7) "Second
"
  [2]=>
  string(6) "Third
"
}
array(3) {
  [0]=>
  string(5) "First"
  [1]=>
  string(6) "Second"
  [2]=>
  string(5) "Third"
}
array(6) {
  [0]=>
  string(5) "First"
  [1]=>
  string(0) ""
  [2]=>
  string(6) "Second"
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(5) "Third"
}
array(6) {
  [0]=>
  string(6) "First
"
  [1]=>
  string(1) "
"
  [2]=>
  string(7) "Second
"
  [3]=>
  string(1) "
"
  [4]=>
  string(1) "
"
  [5]=>
  string(6) "Third
"
}
array(3) {
  [0]=>
  string(7) "First
"
  [1]=>
  string(2) "
"
  [2]=>
  string(8) "Second
"
}
array(2) {
  [0]=>
  string(5) "First"
  [1]=>
  string(6) "Second"
}

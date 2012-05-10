--TEST--
Test file() function : Support for FILE_SKIP_EMPTY_LINES without FILE_IGNORE_NEW_LINES and FILE_INDEX_LINES flags
--FILE--
<?php

$f = tmpnam(__DIR__, "phpfiletest");
file_put_contents($f, "Line 1" . PHP_EOL . "Line 2" . PHP_EOL . PHP_EOL . "Last line...");
$a = file($f, FILE_SKIP_EMPTY_LINES);
$b = file($f, FILE_INDEX_LINES | FILE_SKIP_EMPTY_LINES);

var_dump($a, $b);

@unlink($f);

?>
--EXPECTF--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(12) "Last line..."
}
array(3) {
  [1]=>
  string(7) "Line 1
"
  [2]=>
  string(7) "Line 2
"
  [4]=>
  string(12) "Last line..."
}

--TEST--
GH-24552 (ksort() and krsort() with SORT_FLAG_CASE are locale-dependent)
--SKIPIF--
<?php
if (setlocale(LC_CTYPE, 'tr_TR.UTF-8') === false) {
    die('skip tr_TR.UTF-8 locale not available');
}
?>
--FILE--
<?php
setlocale(LC_CTYPE, 'tr_TR.UTF-8');

$array = ['I' => 0, 'h' => 0];
ksort($array, SORT_STRING | SORT_FLAG_CASE);
var_dump(array_keys($array));
krsort($array, SORT_STRING | SORT_FLAG_CASE);
var_dump(array_keys($array));
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "h"
  [1]=>
  string(1) "I"
}
array(2) {
  [0]=>
  string(1) "I"
  [1]=>
  string(1) "h"
}

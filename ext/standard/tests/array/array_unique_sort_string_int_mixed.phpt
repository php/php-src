--TEST--
array_unique() SORT_STRING preserves string-int equivalence across mixed inputs
--FILE--
<?php
$refInt = 1;
$refStr = '42';
$bigStrThenInt = [];
for ($i = 0; $i < 100; $i++) {
    $bigStrThenInt[] = (string) $i;
}
$bigStrThenInt[] = 50;
$bigStrThenInt[] = 200;
$bigStrThenInt[] = '200';

var_dump(array_unique(['1', 1, '1', '01', 'a', 'a']));
var_dump(array_unique([1, '1', '01', 1.0, '1.0']));
var_dump(array_unique([1.0, 1, '1']));
var_dump(array_unique(['a', 'b', &$refInt, '1', &$refStr, 42]));
var_dump(array_unique(['-5', -5, '-0', 0, '0']));
var_dump(array_unique([PHP_INT_MIN, (string) PHP_INT_MIN, PHP_INT_MAX, (string) PHP_INT_MAX]));
var_dump(array_unique([str_repeat('9', 30), PHP_INT_MAX, str_repeat('9', 30)]));
var_dump(count(array_unique($bigStrThenInt)));
?>
--EXPECTF--
array(3) {
  [0]=>
  string(1) "1"
  [3]=>
  string(2) "01"
  [4]=>
  string(1) "a"
}
array(3) {
  [0]=>
  int(1)
  [2]=>
  string(2) "01"
  [4]=>
  string(3) "1.0"
}
array(1) {
  [0]=>
  float(1)
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  &int(1)
  [4]=>
  &string(2) "42"
}
array(3) {
  [0]=>
  string(2) "-5"
  [2]=>
  string(2) "-0"
  [3]=>
  int(0)
}
array(2) {
  [0]=>
  int(%i)
  [2]=>
  int(%d)
}
array(2) {
  [0]=>
  string(30) "999999999999999999999999999999"
  [1]=>
  int(%d)
}
int(101)

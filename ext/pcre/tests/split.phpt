--TEST--
preg_split()
--FILE--
<?php

var_dump(preg_split('/*/', 'x'));

var_dump(preg_split('/[\s, ]+/', 'x yy,zzz'));
var_dump(preg_split('/[\s, ]+/', 'x yy,zzz', -1));
var_dump(preg_split('/[\s, ]+/', 'x yy,zzz', 0));
var_dump(preg_split('/[\s, ]+/', 'x yy,zzz', 1));
var_dump(preg_split('/[\s, ]+/', 'x yy,zzz', 2));

var_dump(preg_split('/\d*/', 'ab2c3u'));
var_dump(preg_split('/\d*/', 'ab2c3u', -1, PREG_SPLIT_NO_EMPTY));

?>
--EXPECTF--
Warning: preg_split(): Compilation failed: quantifier does not follow a repeatable item at offset 0 in %ssplit.php on line %d
bool(false)
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  string(2) "yy"
  [2]=>
  string(3) "zzz"
}
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  string(2) "yy"
  [2]=>
  string(3) "zzz"
}
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  string(2) "yy"
  [2]=>
  string(3) "zzz"
}
array(1) {
  [0]=>
  string(8) "x yy,zzz"
}
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  string(6) "yy,zzz"
}
array(8) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "b"
  [3]=>
  string(0) ""
  [4]=>
  string(1) "c"
  [5]=>
  string(0) ""
  [6]=>
  string(1) "u"
  [7]=>
  string(0) ""
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "u"
}

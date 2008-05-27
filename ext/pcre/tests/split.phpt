--TEST--
preg_split()
--FILE--
<?php

var_dump(preg_split());
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
Warning: preg_split() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

Warning: preg_split(): Compilation failed: nothing to repeat at offset 0 in %s on line %d
bool(false)
array(3) {
  [0]=>
  unicode(1) "x"
  [1]=>
  unicode(2) "yy"
  [2]=>
  unicode(3) "zzz"
}
array(3) {
  [0]=>
  unicode(1) "x"
  [1]=>
  unicode(2) "yy"
  [2]=>
  unicode(3) "zzz"
}
array(3) {
  [0]=>
  unicode(1) "x"
  [1]=>
  unicode(2) "yy"
  [2]=>
  unicode(3) "zzz"
}
array(1) {
  [0]=>
  unicode(8) "x yy,zzz"
}
array(2) {
  [0]=>
  unicode(1) "x"
  [1]=>
  unicode(6) "yy,zzz"
}
array(8) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(1) "a"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(0) ""
  [4]=>
  unicode(1) "c"
  [5]=>
  unicode(0) ""
  [6]=>
  unicode(1) "u"
  [7]=>
  unicode(0) ""
}
array(4) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(1) "b"
  [2]=>
  unicode(1) "c"
  [3]=>
  unicode(1) "u"
}

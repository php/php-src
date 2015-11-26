--TEST--
Bug #40195 (pcre 6.7 regression)
--FILE--
<?php

var_dump(preg_match('@^(/([a-z]*))*$@', '//abcde', $m)); var_dump($m);
var_dump(preg_match('@^(/(?:[a-z]*))*$@', '//abcde', $m)); var_dump($m);

var_dump(preg_match('@^(/([a-z]+))+$@', '/a/abcde', $m)); var_dump($m);
var_dump(preg_match('@^(/(?:[a-z]+))+$@', '/a/abcde', $m)); var_dump($m);

?>
--EXPECT--
int(1)
array(3) {
  [0]=>
  string(7) "//abcde"
  [1]=>
  string(6) "/abcde"
  [2]=>
  string(5) "abcde"
}
int(1)
array(2) {
  [0]=>
  string(7) "//abcde"
  [1]=>
  string(6) "/abcde"
}
int(1)
array(3) {
  [0]=>
  string(8) "/a/abcde"
  [1]=>
  string(6) "/abcde"
  [2]=>
  string(5) "abcde"
}
int(1)
array(2) {
  [0]=>
  string(8) "/a/abcde"
  [1]=>
  string(6) "/abcde"
}

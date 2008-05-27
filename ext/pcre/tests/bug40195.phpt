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
  unicode(7) "//abcde"
  [1]=>
  unicode(6) "/abcde"
  [2]=>
  unicode(5) "abcde"
}
int(1)
array(2) {
  [0]=>
  unicode(7) "//abcde"
  [1]=>
  unicode(6) "/abcde"
}
int(1)
array(3) {
  [0]=>
  unicode(8) "/a/abcde"
  [1]=>
  unicode(6) "/abcde"
  [2]=>
  unicode(5) "abcde"
}
int(1)
array(2) {
  [0]=>
  unicode(8) "/a/abcde"
  [1]=>
  unicode(6) "/abcde"
}

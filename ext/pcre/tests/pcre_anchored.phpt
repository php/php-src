--TEST--
A (PCRE_ANCHORED) modififer
--FILE--
<?php

var_dump(preg_match('/\PN+/', '123abc', $m));
var_dump($m);

var_dump(preg_match('/\P{N}+/A', '123abc'));
var_dump(preg_match('/^\P{N}+/', '123abc'));
var_dump(preg_match('/^\P{N}+/A', '123abc'));

?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  string(3) "abc"
}
int(0)
int(0)
int(0)
--UEXPECT--
int(1)
array(1) {
  [0]=>
  unicode(3) "abc"
}
int(0)
int(0)
int(0)

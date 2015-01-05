--TEST--
A (PCRE_ANCHORED) modififer
--SKIPIF--
<?php
if (@preg_match_all('/\p{N}/', '0123456789', $dummy) === false) {
	die("skip no support for \p support PCRE library");
}
?>
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

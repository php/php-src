--TEST--
preg_replace() and invalid UTF8 offset
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
	die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php

$string = b"\xc3\xa9 uma string utf8 bem formada";

var_dump(preg_match(b'~.*~u', $string, $m, 0, 1));
var_dump($m);
var_dump(preg_last_error() == PREG_BAD_UTF8_OFFSET_ERROR);

var_dump(preg_match(b'~.*~u', $string, $m, 0, 2));
var_dump($m);
var_dump(preg_last_error() == PREG_NO_ERROR);

echo "Done\n";
?>
--EXPECT--
int(0)
array(0) {
}
bool(true)
int(1)
array(1) {
  [0]=>
  string(28) " uma string utf8 bem formada"
}
bool(true)
Done

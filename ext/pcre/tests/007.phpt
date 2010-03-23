--TEST--
preg_replace_callback() with callback that modifies subject string
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
	die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php

function evil($x) {
	global $txt;
	$txt[3] = "\xFF";
	var_dump($x);
	return $x[0];
}

$txt = "ola123";
var_dump(preg_replace_callback('#.#u', 'evil', $txt));
var_dump($txt);
var_dump(preg_last_error() == PREG_NO_ERROR);

var_dump(preg_replace_callback('#.#u', 'evil', $txt));
var_dump(preg_last_error() == PREG_BAD_UTF8_ERROR);

echo "Done!\n";
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "o"
}
array(1) {
  [0]=>
  string(1) "l"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "3"
}
string(6) "ola123"
string(6) "olaÿ23"
bool(true)
NULL
bool(true)
Done!

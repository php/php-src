--TEST--
preg_match() and invalid UTF8 offset with PREG_NO_UTF8_CHECK
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
	die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php

$string = b"\xc3\xa9 uma string utf8 bem formada";

var_dump(preg_match(b'/.*/u', $string, $m, PREG_NO_UTF8_CHECK, 1));
var_dump(preg_last_error() == PREG_NO_ERROR);
var_dump(count($m));

echo "Done\n";
?>
--EXPECT--
int(1)
bool(true)
int(1)
Done

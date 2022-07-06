--TEST--
Bug #75355 (preg_quote() does not quote # control character)
--FILE--
<?php

var_dump(preg_quote('#'));

var_dump(preg_match('~^(' . preg_quote('hello#world', '~') . ')\z~x', 'hello#world', $m));

var_dump($m[1]);
?>
--EXPECT--
string(2) "\#"
int(1)
string(11) "hello#world"

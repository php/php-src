--TEST--
preg_replace()
--FILE--
<?php

var_dump(preg_replace('{{\D+}}', 'x', '{abcd}'));
var_dump(preg_replace('{{\D+}}', 'ddd', 'abcd'));

var_dump(preg_replace('/(ab)(c)(d)(e)(f)(g)(h)(i)(j)(k)/', 'a${1}2$103', 'zabcdefghijkl'));

var_dump(preg_replace_callback('//e', '', ''));

var_dump(preg_replace_callback('//e', 'strtolower', ''));

?>
--EXPECTF--
string(1) "x"
string(4) "abcd"
string(8) "zaab2k3l"

Warning: preg_replace_callback(): Requires argument 2, '', to be a valid callback in %spreg_replace.php on line 8
string(0) ""

Warning: preg_replace_callback(): Modifier /e cannot be used with replacement callback in %spreg_replace.php on line 10
NULL

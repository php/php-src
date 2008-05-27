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
unicode(1) "x"
unicode(4) "abcd"
unicode(8) "zaab2k3l"

Warning: preg_replace_callback(): Requires argument 2, '', to be a valid callback in %s on line %d
unicode(0) ""

Warning: preg_replace_callback(): Modifier /e cannot be used with replacement callback in %s on line %d
NULL

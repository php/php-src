--TEST--
preg_replace()
--FILE--
<?php

var_dump(preg_replace('{{\D+}}', 'x', '{abcd}'));
var_dump(preg_replace('{{\D+}}', 'ddd', 'abcd'));

var_dump(preg_replace('/(ab)(c)(d)(e)(f)(g)(h)(i)(j)(k)/', 'a${1}2$103', 'zabcdefghijkl'));

?>
--EXPECT--
string(1) "x"
string(4) "abcd"
string(8) "zaab2k3l"

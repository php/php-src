--TEST--
list() with constant keys
--FILE--
<?php

$arr = [
    1 => "one",
    2 => "two",
    3 => "three"
];

const COMPILE_TIME_RESOLVABLE = 1;

define('PROBABLY_NOT_COMPILE_TIME_RESOLVABLE', file_get_contents("data:text/plain,2"));

$probablyNotCompileTimeResolvable3 = cos(0) * 3;

list(
    COMPILE_TIME_RESOLVABLE => $one,
    PROBABLY_NOT_COMPILE_TIME_RESOLVABLE => $two,
    $probablyNotCompileTimeResolvable3 => $three
) = $arr;

var_dump($one, $two, $three);

?>
--EXPECTF--
string(3) "one"
string(3) "two"
string(5) "three"

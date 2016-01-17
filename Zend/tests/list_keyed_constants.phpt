--TEST--
list() with constant keys
--FILE--
<?php

$arr = [
    1 => "one",
    2 => "two"
];

const COMPILE_TIME_RESOLVABLE = 1;

define('PROBABLY_NOT_COMPILE_TIME_RESOLVABLE', file_get_contents("data:text/plain,2"));

list(
    COMPILE_TIME_RESOLVABLE => $one,
    PROBABLY_NOT_COMPILE_TIME_RESOLVABLE => $two
) = $arr;

var_dump($one, $two);

?>
--EXPECTF--
string(3) "one"
string(3) "two"

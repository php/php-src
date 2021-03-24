--TEST--
SCCP: Warning during evaluation of function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test() {
    $array = ["foo", []];
    return implode(",", $array);
}

echo "Before\n";
var_dump(test());
echo "After\n";

?>
--EXPECTF--
Before

Warning: Array to string conversion in %s on line %d
string(9) "foo,Array"
After

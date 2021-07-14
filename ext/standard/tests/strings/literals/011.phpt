--TEST--
Literal Variables implode
--FILE--
<?php
var_dump(
    is_literal(
        implode("delim", [1,2,3])));

var_dump(
    is_literal(
        implode("delim", ["Hello", "World"])));
var_dump(
    is_literal(
        implode(["Hello", "World"])));
?>
--EXPECT--
bool(false)
bool(true)
bool(true)

--TEST--
Literal Variables str_pad
--ENV--
TAINTED=string
--FILE--
<?php
var_dump(
    is_literal(
        str_pad("hello", 20)));
var_dump(
    is_literal(
        str_pad("hello", 20, "pad")));

echo PHP_EOL;
        
var_dump(
    is_literal(
        str_pad($_ENV["TAINTED"], 20)));
var_dump(
    is_literal(
        str_pad("hello", 20, $_ENV["TAINTED"])));
        
echo PHP_EOL;

var_dump(
    is_literal(
        str_pad("hello", 10, 9)));
var_dump(
    is_literal(
        str_pad("hello", 10, 10)));
?>
--EXPECT--
bool(true)
bool(true)

bool(false)
bool(false)

bool(false)
bool(false)

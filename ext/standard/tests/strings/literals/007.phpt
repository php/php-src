--TEST--
Literal Variables in Non Constant Arrays
--ENV--
STR=string
INT=42
--FILE--
<?php
$array = ['aa' => $_ENV["STR"], 'bb' => (int) $_ENV["INT"]];

foreach ($array as $k => $v)
    var_dump(is_literal($k), is_literal($v));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)

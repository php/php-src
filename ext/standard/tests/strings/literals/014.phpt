--TEST--
Literal Variables str_repeat
--ENV--
TAINTED=string
--FILE--
<?php
var_dump(
    is_literal(
        str_repeat("untainted", 2)),
    is_literal(
        str_repeat($_ENV["TAINTED"], 2)),
    is_literal(
        str_repeat(4, 4)), # false
    is_literal(
        str_repeat(42, 4))); # false
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)

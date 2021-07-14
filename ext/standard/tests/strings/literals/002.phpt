--TEST--
Literal Variables Compile Time Concat
--FILE--
<?php
var_dump(
    "literal" . "string",
        is_literal("literal" . "string"),
     24 . "" . 42,
        is_literal(24 . "" . 42));
?>
--EXPECT--
string(13) "literalstring"
bool(true)
string(4) "2442"
bool(false)


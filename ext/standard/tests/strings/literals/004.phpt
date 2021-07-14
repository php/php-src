--TEST--
Literal Variables VM Rope
--ENV--
TAINTED=strings
--FILE--
<?php
$literal = "literal";
$string = "strings";

$twoFour = 24;
$fourTwo = 42;

var_dump(
    "{$literal}{$string}",
        is_literal("{$literal}{$string}"),
    "{$twoFour}{$fourTwo}",
        is_literal("{$twoFour}{$fourTwo}"),
    "not {$literal} {$_ENV["TAINTED"]}",
        is_literal("not {$literal} {$_ENV["TAINTED"]}")
);
?>
--EXPECT--
string(14) "literalstrings"
bool(true)
string(4) "2442"
bool(false)
string(19) "not literal strings"
bool(false)

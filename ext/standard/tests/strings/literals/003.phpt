--TEST--
Literal Variables VM Concat
--ENV--
TAINTED=strings
--FILE--
<?php
$literal = "literal";
$string = "strings";

$twoFour = 24;
$fourTwo = 42;

var_dump(
    $literal . " " . $string,
        is_literal($literal . $string),
    $twoFour . "" . $fourTwo,
        is_literal($twoFour . "" . $fourTwo),
    "not " . "literal " . $_ENV["TAINTED"],
        is_literal("not " . "literal " . $_ENV["TAINTED"]),
     $literal . $string,
        is_literal($literal . $string),
     'literal' . 'strings',
        is_literal('literal' . 'strings'),
);

$literal .= $string;

var_dump(
    $literal,
        is_literal($literal));
        
$literal .= $_ENV["TAINTED"];

var_dump(
    $literal,
        is_literal($literal));
?>
--EXPECT--
string(15) "literal strings"
bool(true)
string(4) "2442"
bool(false)
string(19) "not literal strings"
bool(false)
string(14) "literalstrings"
bool(true)
string(14) "literalstrings"
bool(true)
string(14) "literalstrings"
bool(true)
string(21) "literalstringsstrings"
bool(false)

--TEST--
Markup syntax: bare "<" in operator position keeps comparison/shift/not-equal meaning
--EXTENSIONS--
html
--FILE--
<?php
const FOO = 5;
$a = 1; $b = 2;

// After an operand (variable/constant/literal/")"/"]"), "<" is a comparison.
var_dump($a < $b);      // variable
var_dump($a<$b);        // no surrounding whitespace
var_dump(FOO < 10);     // constant (T_STRING) -- must NOT become a markup tag
var_dump(3 < 4);        // literal
var_dump(($a) < $b);    // ")"
$arr = [9]; var_dump($arr[0] < 10); // "]"

// Other "<" forms are unchanged.
var_dump($a <> $b);     // legacy not-equal
var_dump($a <=> $b);    // spaceship
var_dump($a <= $b);     // less-or-equal
var_dump(1 << 4);       // shift left
var_dump(64 >> 2);      // shift right

// Heredoc (which also starts with "<") is unaffected.
echo <<<EOT
literal <not-markup> text
EOT, "\n";

// Error suppression still works.
$x = []; var_dump(@$x['missing']);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(-1)
bool(true)
int(16)
int(16)
literal <not-markup> text
NULL

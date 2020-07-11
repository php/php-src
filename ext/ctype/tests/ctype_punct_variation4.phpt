--TEST--
Test ctype_punct() function : usage variations - Octal and Hexadecimal values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass different octal and hexadecimal values to ctype_punct() to test behaviour
 */

echo "*** Testing ctype_punct() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

$octal_values = array(041,  042,  043,  044);
$hex_values   = array(0x21, 0x22, 0x23, 0x24);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
    echo "-- Iteration $iterator --\n";
    var_dump(ctype_punct($c));
    $iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
    echo "-- Iteration $iterator --\n";
    var_dump(ctype_punct($c));
    $iterator++;
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_punct() : usage variations ***

-- Octal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

-- Hexadecimal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

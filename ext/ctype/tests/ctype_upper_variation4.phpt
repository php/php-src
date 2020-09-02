--TEST--
Test ctype_upper() function : usage variations - octal and hexadecimal values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass octal and hexadecimal values to ctype_upper() to test behaviour
 */

echo "*** Testing ctype_upper() : usage variations ***\n";
$orig = setlocale(LC_CTYPE, "C");

$octal_values = array(0101, 0102, 0103, 0104);
$hex_values =   array(0x41, 0x42, 0x43, 0x44);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
    echo "-- Iteration $iterator --\n";
    var_dump(ctype_upper($c));
    $iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
    echo "-- Iteration $iterator --\n";
    var_dump(ctype_upper($c));
    $iterator++;
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_upper() : usage variations ***

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

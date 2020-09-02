--TEST--
Test ctype_xdigit() function : usage variations - heaxadecimal and octal values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass different hexadecimal and octal values that:
 * 1. contain hexadecimal digits
 * 2. correspond to character codes recognised as hexadecimal digits (see variation2)
 *    referred to as 'correct' integers below
 */

echo "*** Testing ctype_xdigit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

// contain hexadecimal digits but do not correspond to 'correct' ints
$octal_values1 = array(012, 013, 014, 015);

// correspond to 'correct' integers
$octal_values2 = array(061, 062, 063, 064);

// contain hexadecimal digits but do not correspond to 'correct' ints
$hex_values1 = array(0x1A, 0x1B, 0x1C, 0x1D);

//correspond to 'correct' integers
$hex_values2 = array(0x61, 0x62, 0x63, 0x64);

echo "\n-- Octal values --\n";
echo "'Incorrect' Integers: \n";
foreach($octal_values1 as $c) {
    var_dump(ctype_xdigit($c));
}
echo "'Correct' Integers: \n";
foreach($octal_values2 as $c) {
    var_dump(ctype_xdigit($c));
}

echo "\n-- Hexadecimal values --\n";
echo "'Incorrect' Integers: \n";
foreach($hex_values1 as $c) {
    var_dump(ctype_xdigit($c));
}
echo "'Correct' Integers: \n";
foreach($hex_values2 as $c) {
    var_dump(ctype_xdigit($c));
}
setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_xdigit() : usage variations ***

-- Octal values --
'Incorrect' Integers: 
bool(false)
bool(false)
bool(false)
bool(false)
'Correct' Integers: 
bool(true)
bool(true)
bool(true)
bool(true)

-- Hexadecimal values --
'Incorrect' Integers: 
bool(false)
bool(false)
bool(false)
bool(false)
'Correct' Integers: 
bool(true)
bool(true)
bool(true)
bool(true)

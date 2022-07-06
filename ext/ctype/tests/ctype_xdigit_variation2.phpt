--TEST--
Test ctype_xdigit() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass different integers to ctype_xdigit() to test which character codes are considered
 * valid hexadecimal 'digits'
 */

echo "*** Testing ctype_xdigit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for($c = 1; $c < 256; $c++) {
    if (ctype_xdigit($c)) {
        echo "character code $c is a hexadecimal 'digit'\n";
    }
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_xdigit() : usage variations ***
character code 48 is a hexadecimal 'digit'
character code 49 is a hexadecimal 'digit'
character code 50 is a hexadecimal 'digit'
character code 51 is a hexadecimal 'digit'
character code 52 is a hexadecimal 'digit'
character code 53 is a hexadecimal 'digit'
character code 54 is a hexadecimal 'digit'
character code 55 is a hexadecimal 'digit'
character code 56 is a hexadecimal 'digit'
character code 57 is a hexadecimal 'digit'
character code 65 is a hexadecimal 'digit'
character code 66 is a hexadecimal 'digit'
character code 67 is a hexadecimal 'digit'
character code 68 is a hexadecimal 'digit'
character code 69 is a hexadecimal 'digit'
character code 70 is a hexadecimal 'digit'
character code 97 is a hexadecimal 'digit'
character code 98 is a hexadecimal 'digit'
character code 99 is a hexadecimal 'digit'
character code 100 is a hexadecimal 'digit'
character code 101 is a hexadecimal 'digit'
character code 102 is a hexadecimal 'digit'

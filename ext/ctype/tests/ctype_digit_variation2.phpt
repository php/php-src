--TEST--
Test ctype_digit() function : usage variations - different integers
--EXTENSIONS--
ctype
--FILE--
<?php
/*
 * Pass different integers to ctype_digit() to test which character codes are considered
 * valid decimal digits
 */

echo "*** Testing ctype_digit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
    if (ctype_digit(chr($i))) {
        echo "character code $i is a numeric digit\n";
    }
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_digit() : usage variations ***
character code 48 is a numeric digit
character code 49 is a numeric digit
character code 50 is a numeric digit
character code 51 is a numeric digit
character code 52 is a numeric digit
character code 53 is a numeric digit
character code 54 is a numeric digit
character code 55 is a numeric digit
character code 56 is a numeric digit
character code 57 is a numeric digit

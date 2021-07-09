--TEST--
Test ctype_cntrl() function : usage variations - different integers
--EXTENSIONS--
ctype
--FILE--
<?php
/*
 * Pass different integers to ctype_cntrl() to test which character codes are considered
 * valid control characters
 */

echo "*** Testing ctype_cntrl() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
    if (ctype_cntrl(chr($i))) {
        echo "character code $i is control character\n";
    }
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_cntrl() : usage variations ***
character code 0 is control character
character code 1 is control character
character code 2 is control character
character code 3 is control character
character code 4 is control character
character code 5 is control character
character code 6 is control character
character code 7 is control character
character code 8 is control character
character code 9 is control character
character code 10 is control character
character code 11 is control character
character code 12 is control character
character code 13 is control character
character code 14 is control character
character code 15 is control character
character code 16 is control character
character code 17 is control character
character code 18 is control character
character code 19 is control character
character code 20 is control character
character code 21 is control character
character code 22 is control character
character code 23 is control character
character code 24 is control character
character code 25 is control character
character code 26 is control character
character code 27 is control character
character code 28 is control character
character code 29 is control character
character code 30 is control character
character code 31 is control character
character code 127 is control character

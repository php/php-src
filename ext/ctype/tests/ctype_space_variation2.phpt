--TEST--
Test ctype_space() function : usage variations - different integers
--EXTENSIONS--
ctype
--FILE--
<?php
/*
 * Pass different integers to ctype_space() to test which character codes are considered
 * valid whitespace characters
 */

echo "*** Testing ctype_space() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($c = 1; $c < 256; $c++) {
    if (ctype_space(chr($c))) {
        echo "character code $c is a space character\n";
    }
}
setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_space() : usage variations ***
character code 9 is a space character
character code 10 is a space character
character code 11 is a space character
character code 12 is a space character
character code 13 is a space character
character code 32 is a space character

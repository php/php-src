--TEST--
Test ctype_punct() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass different integers to ctype_punct() to test which character codes are considered
 * valid punctuation characters
 */

echo "*** Testing ctype_punct() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($c = 1; $c < 256; $c++) {
    if (ctype_punct($c)) {
        echo "character code $c is punctuation\n";
    }
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_punct() : usage variations ***
character code 33 is punctuation
character code 34 is punctuation
character code 35 is punctuation
character code 36 is punctuation
character code 37 is punctuation
character code 38 is punctuation
character code 39 is punctuation
character code 40 is punctuation
character code 41 is punctuation
character code 42 is punctuation
character code 43 is punctuation
character code 44 is punctuation
character code 45 is punctuation
character code 46 is punctuation
character code 47 is punctuation
character code 58 is punctuation
character code 59 is punctuation
character code 60 is punctuation
character code 61 is punctuation
character code 62 is punctuation
character code 63 is punctuation
character code 64 is punctuation
character code 91 is punctuation
character code 92 is punctuation
character code 93 is punctuation
character code 94 is punctuation
character code 95 is punctuation
character code 96 is punctuation
character code 123 is punctuation
character code 124 is punctuation
character code 125 is punctuation
character code 126 is punctuation

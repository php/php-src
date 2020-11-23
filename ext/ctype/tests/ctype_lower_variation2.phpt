--TEST--
Test ctype_lower() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass different integers to ctype_lower() to test which character codes are considered
 * valid lowercase characters
 */

echo "*** Testing ctype_lower() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
    if (ctype_lower($i)) {
        echo "character code $i is a lower case character\n";
    }
}

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_lower() : usage variations ***
character code 97 is a lower case character
character code 98 is a lower case character
character code 99 is a lower case character
character code 100 is a lower case character
character code 101 is a lower case character
character code 102 is a lower case character
character code 103 is a lower case character
character code 104 is a lower case character
character code 105 is a lower case character
character code 106 is a lower case character
character code 107 is a lower case character
character code 108 is a lower case character
character code 109 is a lower case character
character code 110 is a lower case character
character code 111 is a lower case character
character code 112 is a lower case character
character code 113 is a lower case character
character code 114 is a lower case character
character code 115 is a lower case character
character code 116 is a lower case character
character code 117 is a lower case character
character code 118 is a lower case character
character code 119 is a lower case character
character code 120 is a lower case character
character code 121 is a lower case character
character code 122 is a lower case character

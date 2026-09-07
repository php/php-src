--TEST--
grapheme_strrev() stops at UBRK_DONE instead of using it as a byte index
--EXTENSIONS--
intl
--FILE--
<?php

$cases = [
    'abc',
    'a',
    '土下座',
    "null\x00byte",
];

foreach ($cases as $s) {
    $rev = grapheme_strrev($s);
    echo strlen($s), ' ', strlen($rev), ' ', bin2hex($rev), "\n";
}

?>
--EXPECT--
3 3 636261
1 1 61
9 9 e5baa7e4b88be59c9f
9 9 65747962006c6c756e

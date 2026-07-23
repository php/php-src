--TEST--
grapheme() str[i]pos limits
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
try {
    grapheme_stripos(1,1,2147483648);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), PHP_EOL;
}

try {
    grapheme_strpos(1,1,2147483648);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ValueError: grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

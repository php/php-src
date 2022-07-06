--TEST--
mb_str_split() error conditions
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

$string = "日本"; /* 2 chars */

// Invalid split length
try {
    mb_str_split($string, 0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    mb_str_split($string, -5);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

//Invalid Encoding
try {
    mb_str_split($string, 1, "BAD_ENCODING");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_str_split(): Argument #2 ($length) must be greater than 0
mb_str_split(): Argument #2 ($length) must be greater than 0
mb_str_split(): Argument #3 ($encoding) must be a valid encoding, "BAD_ENCODING" given

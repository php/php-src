--TEST--
GH-20833 (mb_str_pad() divide by zero if padding string is invalid in the encoding)
--FILE--
<?php
$utf8 = "test";
$utf32 = mb_convert_encoding($utf8, 'UTF-32', 'UTF-8');
try {
    mb_str_pad($utf32, 5, "1" /* invalid for encoding */, STR_PAD_RIGHT, "UTF-32");
} catch (ValueError $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: mb_str_pad(): Argument #3 ($pad_string) must not be empty

--TEST--
grapheme_strpos() family with empty needle and offset on multi-code-unit graphemes
--EXTENSIONS--
intl
--FILE--
<?php

var_dump(grapheme_strpos("😀x", ""));
var_dump(grapheme_strpos("😀x", "", 0));
var_dump(grapheme_strpos("😀x", "", 1));
var_dump(grapheme_stripos("😀x", "", 1));
var_dump(grapheme_strpos("😀x", "", -1));
var_dump(grapheme_strrpos("😀x", ""));
var_dump(grapheme_strrpos("😀x", "", 1));
var_dump(grapheme_strripos("😀x", "", 1));
var_dump(grapheme_strrpos("😀x", "", -1));
try {
    var_dump(grapheme_strpos("😀x", "", 5));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(0)
int(0)
int(1)
int(1)
int(1)
int(2)
int(2)
int(2)
int(1)
ValueError: grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

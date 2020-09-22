--TEST--
grapheme_* functions with out-of-bounds offsets
--FILE--
<?php

// Offset == Length is legal.
var_dump(grapheme_strpos("foo", "bar", 3));
var_dump(grapheme_stripos("foo", "bar", 3));
var_dump(grapheme_strrpos("foo", "bar", 3));
var_dump(grapheme_strripos("foo", "bar", 3));
echo "\n";

// Offset == -Length is legal.
var_dump(grapheme_strpos("foo", "bar", -3));
var_dump(grapheme_stripos("foo", "bar", -3));
var_dump(grapheme_strrpos("foo", "bar", -3));
var_dump(grapheme_strripos("foo", "bar", -3));
echo "\n";

// Positive out of bounds.
try {
    var_dump(grapheme_strpos("foo", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("foo", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("foo", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("foo", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

// Negative out of bounds.
try {
    var_dump(grapheme_strpos("foo", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("foo", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("foo", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("foo", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

// TODO: substr is special.
var_dump(grapheme_substr("foo", 3));
var_dump(grapheme_substr("foo", -3));
//var_dump(grapheme_substr("foo", 4));
//var_dump(grapheme_substr("foo", -4));

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)

bool(false)
bool(false)
bool(false)
bool(false)

grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

bool(false)
string(3) "foo"

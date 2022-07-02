--TEST--
grapheme_* functions with out-of-bounds offsets
--EXTENSIONS--
intl
--FILE--
<?php

// Offset == Length is legal.
var_dump(grapheme_strpos("foo", "bar", 3));
var_dump(grapheme_stripos("foo", "bar", 3));
var_dump(grapheme_strrpos("foo", "bar", 3));
var_dump(grapheme_strripos("foo", "bar", 3));
var_dump(grapheme_strpos("äöü", "bar", 3));
var_dump(grapheme_stripos("äöü", "bar", 3));
var_dump(grapheme_strrpos("äöü", "bar", 3));
var_dump(grapheme_strripos("äöü", "bar", 3));
echo "\n";

// Offset == -Length is legal.
var_dump(grapheme_strpos("foo", "bar", -3));
var_dump(grapheme_stripos("foo", "bar", -3));
var_dump(grapheme_strrpos("foo", "bar", -3));
var_dump(grapheme_strripos("foo", "bar", -3));
var_dump(grapheme_strpos("äöü", "bar", -3));
var_dump(grapheme_stripos("äöü", "bar", -3));
var_dump(grapheme_strrpos("äöü", "bar", -3));
var_dump(grapheme_strripos("äöü", "bar", -3));
echo "\n";

// Offset == Length is legal.
var_dump(grapheme_strpos("", "bar", 0));
var_dump(grapheme_stripos("", "bar", 0));
var_dump(grapheme_strrpos("", "bar", 0));
var_dump(grapheme_strripos("", "bar", 0));
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
try {
    var_dump(grapheme_strpos("äöü", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("äöü", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("äöü", "bar", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("äöü", "bar", 4));
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
try {
    var_dump(grapheme_strpos("äöü", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("äöü", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("äöü", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("äöü", "bar", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

// Empty needle +  out of bounds
try {
    var_dump(grapheme_strpos("äöü", "", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("äöü", "", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("äöü", "", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("äöü", "", 4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strpos("äöü", "", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_stripos("äöü", "", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strrpos("äöü", "", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(grapheme_strripos("äöü", "", -4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

var_dump(grapheme_substr("foo", 3));
var_dump(grapheme_substr("foo", -3));
var_dump(grapheme_substr("foo", 4));
var_dump(grapheme_substr("foo", -4));
var_dump(grapheme_substr("äöü", 3));
var_dump(grapheme_substr("äöü", -3));
var_dump(grapheme_substr("äöü", 4));
var_dump(grapheme_substr("äöü", -4));
var_dump(grapheme_substr("foo", 0, 3));
var_dump(grapheme_substr("foo", 0, -3));
var_dump(grapheme_substr("foo", 0, 4));
var_dump(grapheme_substr("foo", 0, -4));
var_dump(grapheme_substr("äöü", 0, 3));
var_dump(grapheme_substr("äöü", 0, -3));
var_dump(grapheme_substr("äöü", 0, 4));
var_dump(grapheme_substr("äöü", 0, -4));
var_dump(grapheme_substr("äöü", -4, 1));
var_dump(grapheme_substr("äöü", -4, -1));
var_dump(grapheme_substr("äöü", 2, -2));

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

bool(false)
bool(false)
bool(false)
bool(false)
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

grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
grapheme_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

string(0) ""
string(3) "foo"
string(0) ""
string(3) "foo"
string(0) ""
string(6) "äöü"
string(0) ""
string(6) "äöü"
string(3) "foo"
string(0) ""
string(3) "foo"
string(0) ""
string(6) "äöü"
string(0) ""
string(6) "äöü"
string(0) ""
string(2) "ä"
string(4) "äö"
string(0) ""

--TEST--
Hash: hash_equals() test
--FILE--
<?php

    require_once __DIR__ . '/exceptional.inc';

    trycatch_dump(fn() => hash_equals("same", "same"));
    trycatch_dump(fn() => hash_equals("not1same", "not2same"));
    trycatch_dump(fn() => hash_equals("short", "longer"));
    trycatch_dump(fn() => hash_equals("longer", "short"));
    trycatch_dump(fn() => hash_equals("", "notempty"));
    trycatch_dump(fn() => hash_equals("notempty", ""));
    trycatch_dump(fn() => hash_equals("", ""));
    trycatch_dump(fn() => hash_equals(123, "NaN"));
    trycatch_dump(fn() => hash_equals("NaN", 123));
    trycatch_dump(fn() => hash_equals(123, 123));
    trycatch_dump(fn() => hash_equals(null, ""));
    trycatch_dump(fn() => hash_equals(null, 123));
    trycatch_dump(fn() => hash_equals(null, null));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
[ErrorException] Parameter 1 is invalid: Expected known_string to be a string, int given
[ErrorException] Parameter 2 is invalid: Expected user_string to be a string, int given
[ErrorException] Parameter 1 is invalid: Expected known_string to be a string, int given
[ErrorException] Parameter 1 is invalid: Expected known_string to be a string, null given
[ErrorException] Parameter 1 is invalid: Expected known_string to be a string, null given
[ErrorException] Parameter 1 is invalid: Expected known_string to be a string, null given

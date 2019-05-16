--TEST--
Hash: hash_equals() test
--FILE--
<?php
var_dump(hash_equals("same", "same"));
var_dump(hash_equals("not1same", "not2same"));
var_dump(hash_equals("short", "longer"));
var_dump(hash_equals("longer", "short"));
var_dump(hash_equals("", "notempty"));
var_dump(hash_equals("notempty", ""));
var_dump(hash_equals("", ""));
var_dump(hash_equals(123, "NaN"));
var_dump(hash_equals("NaN", 123));
var_dump(hash_equals(123, 123));
var_dump(hash_equals(null, ""));
var_dump(hash_equals(null, 123));
var_dump(hash_equals(null, null));
?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

Warning: hash_equals(): Expected known_string to be a string, int given in %s on line %d
bool(false)

Warning: hash_equals(): Expected user_string to be a string, int given in %s on line %d
bool(false)

Warning: hash_equals(): Expected known_string to be a string, int given in %s on line %d
bool(false)

Warning: hash_equals(): Expected known_string to be a string, null given in %s on line %d
bool(false)

Warning: hash_equals(): Expected known_string to be a string, null given in %s on line %d
bool(false)

Warning: hash_equals(): Expected known_string to be a string, null given in %s on line %d
bool(false)

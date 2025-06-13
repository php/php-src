--TEST--
str_ends_with() function - unit tests for str_ends_with()
--FILE--
<?php
declare(strict_types=1);
$testStr = "beginningMiddleEnd";
var_dump(str_ends_with($testStr, "End"));
var_dump(str_ends_with($testStr, "end"));
var_dump(str_ends_with($testStr, "en"));
var_dump(str_ends_with($testStr, $testStr));
var_dump(str_ends_with($testStr, $testStr.$testStr));
var_dump(str_ends_with($testStr, ""));
var_dump(str_ends_with("", ""));
var_dump(str_ends_with("", " "));
var_dump(str_ends_with($testStr, "\x00"));
var_dump(str_ends_with("\x00", ""));
var_dump(str_ends_with("\x00", "\x00"));
var_dump(str_ends_with("a\x00", "\x00"));
var_dump(str_ends_with("ab\x00c", "b\x00c"));
var_dump(str_ends_with("a\x00b", "d\x00b"));
var_dump(str_ends_with("a\x00b", "a\x00z"));
var_dump(str_ends_with("a", "\x00a"));
var_dump(str_ends_with("a", "a\x00"));
// now test varargs
if (!str_ends_with("foo", "a", "o")) {
    throw new \Exception("str does not end with o");
}
try {
    if (!str_ends_with("foo1", "a", "o", 1, "x")) {
        throw new \Exception("str does not end with 1");
    }
    throw new \Exception("int(1) did not trigger TypeError");
} catch (\TypeError $e) {
    // Expected TypeError due to int(1) + strict_types
}
if (!str_ends_with("foo1", "a", "o", "1", "x")) {
    throw new \Exception("str does not end with 1");
}
if (str_ends_with("", "a", "o")) {
    throw new \Exception("Empty string ends with something");
}
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)

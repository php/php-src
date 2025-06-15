--TEST--
str_starts_with() function - unit tests for str_starts_with()
--FILE--
<?php
declare(strict_types=1);
$testStr = "beginningMiddleEnd";
var_dump(str_starts_with($testStr, "beginning"));
var_dump(str_starts_with($testStr, "Beginning"));
var_dump(str_starts_with($testStr, "eginning"));
var_dump(str_starts_with($testStr, $testStr));
var_dump(str_starts_with($testStr, $testStr.$testStr));
var_dump(str_starts_with($testStr, ""));
var_dump(str_starts_with("", ""));
var_dump(str_starts_with("", " "));
var_dump(str_starts_with($testStr, "\x00"));
var_dump(str_starts_with("\x00", ""));
var_dump(str_starts_with("\x00", "\x00"));
var_dump(str_starts_with("\x00a", "\x00"));
var_dump(str_starts_with("a\x00bc", "a\x00b"));
var_dump(str_starts_with("a\x00b", "a\x00d"));
var_dump(str_starts_with("a\x00b", "z\x00b"));
var_dump(str_starts_with("a", "a\x00"));
var_dump(str_starts_with("a", "\x00a"));
// now test varargs
if (!str_starts_with("foo", "a", "f")) {
    throw new \Exception("str does not start with f");
}
try {
    if (!str_starts_with("1foo", "a", "f", 1, "x")) {
        throw new \Exception("str does not start with 1");
    }
    throw new \Exception("int(1) did not trigger TypeError");
} catch (\TypeError $e) {
    // Expected TypeError due to int(1) + strict_types
}
if (!str_starts_with("1foo", "a", "f", "1", "x")) {
    throw new \Exception("str does not start with 1");
}
if (str_starts_with("", "a", "f")) {
    throw new \Exception("Empty string starts with something");
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

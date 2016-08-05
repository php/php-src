--TEST--
filter_assert() and FILTER_VALIDATE_BOOLEAN
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

class test {

    function __toString() {
        return "blah";
    }
}

$t = new test;

try {
var_dump(filter_assert("no", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert(NULL, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert($t, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert(array(1,2,3,0,array("", "123")), FILTER_VALIDATE_BOOLEAN, FILTER_REQUIRE_ARRAY));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("yes", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("true", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("false", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("off", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("on", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("0", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("1", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("NONE", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert(-1, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("000000", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_assert("111111", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(false)
string(29) "Bool validation: Invalid bool"
string(29) "Bool validation: Invalid bool"
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
string(29) "Bool validation: Invalid bool"
bool(false)
string(29) "Bool validation: Invalid bool"
string(29) "Bool validation: Invalid bool"
string(29) "Bool validation: Invalid bool"
Done

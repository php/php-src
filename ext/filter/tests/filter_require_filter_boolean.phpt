--TEST--
filter_require_var() and FILTER_VALIDATE_BOOLEAN
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
var_dump(filter_require_var("no", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var(NULL, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var($t, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var(array(1,2,3,0,array("", "123")), FILTER_VALIDATE_BOOLEAN, FILTER_REQUIRE_ARRAY));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("yes", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("true", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("false", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("off", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("on", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("0", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("1", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("NONE", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var(-1, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("000000", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(filter_require_var("111111", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECT--
bool(false)
string(93) "Bool validation: Empty input (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 16777216)"
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(93) "Bool validation: Empty input (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
string(94) "Bool validation: Invalid bool (invalid_key: N/A, filter_name: boolean, filter_flags: 33554432)"
Done

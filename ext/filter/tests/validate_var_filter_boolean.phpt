--TEST--
validate_var() and FILTER_VALIDATE_BOOLEAN
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
var_dump(validate_var("no", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var(NULL, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var($t, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var(array(1,2,3,0,array("", "123")), FILTER_VALIDATE_BOOLEAN, FILTER_REQUIRE_ARRAY));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("yes", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("true", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("false", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("off", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("on", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("0", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("1", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("NONE", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var(-1, FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("000000", FILTER_VALIDATE_BOOLEAN));
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
var_dump(validate_var("111111", FILTER_VALIDATE_BOOLEAN));
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

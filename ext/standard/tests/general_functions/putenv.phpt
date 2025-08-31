--TEST--
putenv() basic tests
--FILE--
<?php

$var_name="SUCHVARSHOULDNOTEXIST";

var_dump(getenv($var_name));
var_dump(putenv($var_name."=value"));
var_dump(getenv($var_name));

var_dump(putenv($var_name."="));
var_dump(getenv($var_name));

var_dump(putenv($var_name));
var_dump(getenv($var_name));

try {
    putenv("=123");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    putenv("");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(true)
string(5) "value"
bool(true)
string(0) ""
bool(true)
bool(false)
putenv(): Argument #1 ($assignment) must have a valid syntax
putenv(): Argument #1 ($assignment) must have a valid syntax
Done

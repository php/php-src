--TEST--
Integer overflow
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$max = sprintf("%d", PHP_INT_MAX);
switch($max) {
case "2147483647": /* 32-bit systems */
    $min = "-2147483648";
    $overflow = "2147483648";
    $underflow = "-2147483649";
    break;
case "9223372036854775807": /* 64-bit systems */
    $min = "-9223372036854775808";
    $overflow = "9223372036854775808";
    $underflow = "-9223372036854775809";
    break;
default:
    die("failed: unknown value for PHP_MAX_INT");
    break;
}

function test_validation($val, $msg) {
    $f = filter_var($val, FILTER_VALIDATE_INT);
    echo "$msg filtered: "; var_dump($f); // filtered value (or false)
    echo "$msg is_long: "; var_dump(is_long($f)); // test validation
    echo "$msg equal: "; var_dump($val == $f); // test equality of result
}

// PHP_INT_MAX
test_validation($max, "max");
test_validation($overflow, "overflow");
test_validation($min, "min");
test_validation($underflow, "underflow");
?>
--EXPECTF--
max filtered: int(%d)
max is_long: bool(true)
max equal: bool(true)
overflow filtered: bool(false)
overflow is_long: bool(false)
overflow equal: bool(false)
min filtered: int(-%d)
min is_long: bool(true)
min equal: bool(true)
underflow filtered: bool(false)
underflow is_long: bool(false)
underflow equal: bool(false)

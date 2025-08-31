--TEST--
Increment/decrement a typed property with int|float type
--FILE--
<?php

class Test {
    public int|float $prop;
    public int|bool $prop2;
}

/* Incrementing a int|float property past int min/max is legal */

$test = new Test;
$test->prop = PHP_INT_MAX;
$x = $test->prop++;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MAX;
$x = ++$test->prop;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MIN;
$x = $test->prop--;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MIN;
$x = --$test->prop;
var_dump(is_double($test->prop));

$test = new Test;
$test->prop = PHP_INT_MAX;
$r =& $test->prop;
$x = $test->prop++;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MAX;
$x = ++$test->prop;
$r =& $test->prop;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MIN;
$x = $test->prop--;
$r =& $test->prop;
var_dump(is_double($test->prop));

$test->prop = PHP_INT_MIN;
$x = --$test->prop;
$r =& $test->prop;
var_dump(is_double($test->prop));

/* Incrementing a non-int|float property past int min/max is an error,
 * even if the result of the overflow (a float) would technically be allowed
 * under a type coercion. */

try {
    $test->prop2 = PHP_INT_MAX;
    $x = $test->prop2++;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MAX;
    $x = ++$test->prop2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MIN;
    $x = $test->prop2--;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MIN;
    $x = --$test->prop2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MAX;
    $r =& $test->prop2;
    $x = $test->prop2++;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MAX;
    $r =& $test->prop2;
    $x = ++$test->prop2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MIN;
    $r =& $test->prop2;
    $x = $test->prop2--;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $test->prop2 = PHP_INT_MIN;
    $r =& $test->prop2;
    $x = --$test->prop2;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Cannot increment property Test::$prop2 of type int|bool past its maximal value
Cannot increment property Test::$prop2 of type int|bool past its maximal value
Cannot decrement property Test::$prop2 of type int|bool past its minimal value
Cannot decrement property Test::$prop2 of type int|bool past its minimal value
Cannot increment a reference held by property Test::$prop2 of type int|bool past its maximal value
Cannot increment a reference held by property Test::$prop2 of type int|bool past its maximal value
Cannot decrement a reference held by property Test::$prop2 of type int|bool past its minimal value
Cannot decrement a reference held by property Test::$prop2 of type int|bool past its minimal value

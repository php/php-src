--TEST--
Test sizeof() function : basic functionality - for scalar types
--FILE--
<?php
/**
 * Prototype  : int sizeof(array|Countable|null $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 *  If Standard PHP library is installed, it will return the properties of an object.
 * Source code: ext/standard/array.c
 * Alias to functions: count()
 */

/* Testing the sizeof() for some of the scalar types(integer, float) values
 * in default, COUNT_NORMAL and COUNT_RECURSIVE modes.
 */

echo "*** Testing sizeof() : basic functionality ***\n";

$intval = 10;
$floatval = 10.5;
$stringval = "String";

echo "-- Testing sizeof() for integer type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --\n";

echo "default mode: ";
try {
    var_dump( sizeof($intval) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n";

echo "COUNT_NORMAL mode: ";
try {
    var_dump( sizeof($intval, COUNT_NORMAL) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n";

echo "COUNT_RECURSIVE mode: ";
try {
    var_dump( sizeof($intval, COUNT_RECURSIVE) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n";

echo "-- Testing sizeof() for float  type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --\n";

echo "default mode: ";
try {
    var_dump( sizeof($floatval) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n";

echo "COUNT_NORMAL mode: ";
try {
    var_dump( sizeof($floatval, COUNT_NORMAL) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
echo "\n";

echo "COUNT_RECURSIVE mode: ";
try {
    var_dump( sizeof($floatval, COUNT_RECURSIVE) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>

DONE
--EXPECT--
*** Testing sizeof() : basic functionality ***
-- Testing sizeof() for integer type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --
default mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE mode: Parameter must be an array or an object that implements Countable

-- Testing sizeof() for float  type in default, COUNT_NORMAL and COUNT_RECURSIVE modes --
default mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE mode: Parameter must be an array or an object that implements Countable

DONE

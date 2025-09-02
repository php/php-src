--TEST--
Test pow() function : usage variations - different data types as $base argument
--INI--
precision = 14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
class classA
{
}

// resource variable
$fp = fopen(__FILE__, "r");

$inputs = [
    // int data
    0,
    1,
    12345,
    -2345,
    PHP_INT_MAX,

    // float data
    10.5,
    -10.5,
    12.3456789e10,
    12.3456789e-10,
    0.5,

    // null data
    null,

    // boolean data
    true,
    false,

    // empty data
    "",
    [],

    // string data
    "abcxyz",
    "10.5",
    "2",
    "6.3e-2",

    // object data
    new classA(),

    // resource variable
    $fp,
];

// loop through each element of $inputs to check the behaviour of pow()
foreach ($inputs as $input) {
    try {
        var_dump(pow($input, 3));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
fclose($fp);
?>
--EXPECT--
int(0)
int(1)
int(1881365963625)
int(-12895213625)
float(7.846377169233351E+56)
float(1157.625)
float(-1157.625)
float(1.8816763717891549E+33)
float(1.8816763717891545E-27)
float(0.125)
int(0)
int(1)
int(0)
Unsupported operand types: string ** int
Unsupported operand types: array ** int
Unsupported operand types: string ** int
float(1157.625)
int(8)
float(0.000250047)
Unsupported operand types: classA ** int
Unsupported operand types: resource ** int

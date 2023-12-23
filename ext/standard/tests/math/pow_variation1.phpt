--TEST--
Test pow() function : usage variations - different data types as $base argument
--INI--
serialize_precision = 14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
echo "*** Testing pow() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset($unset_var);

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a class
class classA
{
}

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = [
    // int data
    /*1*/ 0,
    1,
    12345,
    -2345,
    PHP_INT_MAX,

    // float data
    /*6*/ 10.5,
    -10.5,
    12.3456789e10,
    12.3456789e-10,
    0.5,

    // null data
    /*11*/ null,
    null,

    // boolean data
    /*13*/ true,
    false,
    true,
    false,

    // empty data
    /*17*/ "",
    "",
    [],

    // string data
    /*20*/ "abcxyz",
    "abcxyz",
    $heredoc,
    "5.5",
    "2",

    // object data
    /*25*/ new classA(),

    // undefined data
    /*26*/ @$undefined_var,

    // unset data
    /*27*/ @$unset_var,

    // resource variable
    /*28*/ $fp,
];

// loop through each element of $inputs to check the behaviour of pow()
$iterator = 1;
foreach ($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(pow($input, 3));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
}
fclose($fp);

?>
--EXPECTF--
*** Testing pow() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
float(1881365963625)

-- Iteration 4 --
float(-12895213625)

-- Iteration 5 --
float(9.903520300448E+27)

-- Iteration 6 --
float(1157.625)

-- Iteration 7 --
float(-1157.625)

-- Iteration 8 --
float(1.881676371789%dE+33)

-- Iteration 9 --
float(1.881676371789%dE-27)

-- Iteration 10 --
float(0.125)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(0)

-- Iteration 13 --
int(1)

-- Iteration 14 --
int(0)

-- Iteration 15 --
int(1)

-- Iteration 16 --
int(0)

-- Iteration 17 --
Unsupported operand types: string ** int

-- Iteration 18 --
Unsupported operand types: string ** int

-- Iteration 19 --
Unsupported operand types: array ** int

-- Iteration 20 --
Unsupported operand types: string ** int

-- Iteration 21 --
Unsupported operand types: string ** int

-- Iteration 22 --
Unsupported operand types: string ** int

-- Iteration 23 --
float(15532029.564086)

-- Iteration 24 --
float(412.09)

-- Iteration 25 --
Unsupported operand types: float ** classA

-- Iteration 26 --
float(1)

-- Iteration 27 --
float(1)

-- Iteration 28 --
Unsupported operand types: float ** resource

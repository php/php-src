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
unset ($unset_var);

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

$inputs = array(
       // int data
/*1*/  0,
       1,
       12345,
       -2345,
       PHP_INT_MAX,

       // float data
/*6*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*11*/ NULL,
       null,

       // boolean data
/*13*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*17*/ "",
       '',
       array(),

       // string data
/*20*/ "abcxyz",
       'abcxyz',
       $heredoc,

       // object data
/*23*/ new classA(),

       // undefined data
/*24*/ @$undefined_var,

       // unset data
/*25*/ @$unset_var,

       // resource variable
/*26*/ $fp
);

// loop through each element of $inputs to check the behaviour of pow()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(pow($input, 3));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};
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
Unsupported operand types: classA ** int

-- Iteration 24 --
int(0)

-- Iteration 25 --
int(0)

-- Iteration 26 --
Unsupported operand types: resource ** int

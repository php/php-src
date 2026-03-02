--TEST--
Test round() function : usage variations - different data types as $val argument
--INI--
precision=14
--FILE--
<?php
echo "*** Testing round() : usage variations ***\n";

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
       2147483647,

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

// loop through each element of $inputs to check the behaviour of round()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(round($input, 14));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
--EXPECTF--
*** Testing round() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(1)

-- Iteration 3 --
float(12345)

-- Iteration 4 --
float(-2345)

-- Iteration 5 --
float(2147483647)

-- Iteration 6 --
float(10.5)

-- Iteration 7 --
float(-10.5)

-- Iteration 8 --
float(123456789000)

-- Iteration 9 --
float(1.23457E-9)

-- Iteration 10 --
float(0.5)

-- Iteration 11 --

Deprecated: round(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 12 --

Deprecated: round(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 13 --
float(1)

-- Iteration 14 --
float(0)

-- Iteration 15 --
float(1)

-- Iteration 16 --
float(0)

-- Iteration 17 --
round(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 18 --
round(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 19 --
round(): Argument #1 ($num) must be of type int|float, array given

-- Iteration 20 --
round(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 21 --
round(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 22 --
round(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 23 --
round(): Argument #1 ($num) must be of type int|float, classA given

-- Iteration 24 --

Deprecated: round(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 25 --

Deprecated: round(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 26 --
round(): Argument #1 ($num) must be of type int|float, resource given

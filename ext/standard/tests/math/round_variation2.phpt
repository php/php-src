--TEST--
Test round() function : usage variations - different data types as $precision argument
--INI--
precision=14
--FILE--
<?php
/* Prototype  : float round  ( float $val  [, int $precision  ] )
 * Description: Returns the rounded value of val  to specified precision (number of digits
 * after the decimal point)
 * Source code: ext/standard/math.c
 */

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
       12.3456789000e5,
       12.3456789000E-5,
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
	var_dump(round(123.4456789, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing round() : usage variations ***

-- Iteration 1 --
float(123)

-- Iteration 2 --
float(123.4)

-- Iteration 3 --
float(123.4456789)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(123.4456789)

-- Iteration 6 --
float(123.4456789)

-- Iteration 7 --
float(0)

-- Iteration 8 --
float(123.4456789)

-- Iteration 9 --
float(123)

-- Iteration 10 --
float(123)

-- Iteration 11 --
float(123)

-- Iteration 12 --
float(123)

-- Iteration 13 --
float(123.4)

-- Iteration 14 --
float(123)

-- Iteration 15 --
float(123.4)

-- Iteration 16 --
float(123)

-- Iteration 17 --

Warning: round() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: round() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: round() expects parameter 2 to be integer, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: round() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: round() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: round() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: round() expects parameter 2 to be integer, object given in %s on line %d
NULL

-- Iteration 24 --
float(123)

-- Iteration 25 --
float(123)

-- Iteration 26 --

Warning: round() expects parameter 2 to be integer, resource given in %s on line %d
NULL
===Done===
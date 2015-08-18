--TEST--
Test hypot() function : usage variations - different data types as $x argument
--FILE--
<?php
/* Prototype  : float hypot  ( float $x  , float $y  )
 * Description: Calculate the length of the hypotenuse of a right-angle triangle
 * Source code: ext/standard/math.c
 */

echo "*** Testing hypot() : usage variations ***\n";

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

// unexpected values to be passed to $arg argument
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

// loop through each element of $inputs to check the behaviour of hypot()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(hypot($input, 5));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing hypot() : usage variations ***

-- Iteration 1 --
float(5)

-- Iteration 2 --
float(5.0990195135928)

-- Iteration 3 --
float(12345.001012556)

-- Iteration 4 --
float(2345.0053304843)

-- Iteration 5 --
float(2147483647)

-- Iteration 6 --
float(11.629703349613)

-- Iteration 7 --
float(11.629703349613)

-- Iteration 8 --
float(123456789000)

-- Iteration 9 --
float(5)

-- Iteration 10 --
float(5.0249378105604)

-- Iteration 11 --
float(5)

-- Iteration 12 --
float(5)

-- Iteration 13 --
float(5.0990195135928)

-- Iteration 14 --
float(5)

-- Iteration 15 --
float(5.0990195135928)

-- Iteration 16 --
float(5)

-- Iteration 17 --

Warning: hypot() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: hypot() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: hypot() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: hypot() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: hypot() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: hypot() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: hypot() expects parameter 1 to be float, object given in %s on line %d
NULL

-- Iteration 24 --
float(5)

-- Iteration 25 --
float(5)

-- Iteration 26 --

Warning: hypot() expects parameter 1 to be float, resource given in %s on line %d
NULL
===Done===
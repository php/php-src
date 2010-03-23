--TEST--
Test exp() function : usage variations - different data types as $arg argument
--INI--
precision=14
--FILE--
<?php
/* Prototype  : float exp  ( float $arg  )
 * Description: Returns e raised to the power of arg.
 * Source code: ext/standard/math.c
 */

echo "*** Testing exp() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of exp()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(exp($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing exp() : usage variations ***

-- Iteration 1 --
float(1)

-- Iteration 2 --
float(2.718281828459)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(INF)

-- Iteration 6 --
float(36315.502674247)

-- Iteration 7 --
float(2.7536449349747E-5)

-- Iteration 8 --
float(INF)

-- Iteration 9 --
float(1.0000000012346)

-- Iteration 10 --
float(1.6487212707001)

-- Iteration 11 --
float(1)

-- Iteration 12 --
float(1)

-- Iteration 13 --
float(2.718281828459)

-- Iteration 14 --
float(1)

-- Iteration 15 --
float(2.718281828459)

-- Iteration 16 --
float(1)

-- Iteration 17 --

Warning: exp() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: exp() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: exp() expects parameter 1 to be double, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: exp() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: exp() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: exp() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: exp() expects parameter 1 to be double, object given in %s on line %d
NULL

-- Iteration 24 --
float(1)

-- Iteration 25 --
float(1)

-- Iteration 26 --

Warning: exp() expects parameter 1 to be double, resource given in %s on line %d
NULL
===Done===

--TEST--
Test atan2() function : usage variations - different data types as $x arg
--INI--
precision = 10
--FILE--
<?php
/* Prototype  : float atan2  ( float $y  , float $x  )
 * Description: Arc tangent of two variables.
 * Source code: ext/standard/math.c
 */

echo "*** Testing atan2() : basic functionality ***\n";

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
       12,
       -12,
       2147483647,

       // float data
/*6*/  10.5,
       -10.5,
       1.234567e2,
       1.234567E-2,
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

// loop through each element of $inputs to check the behaviour of atan2()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(atan2(23, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing atan2() : basic functionality ***

-- Iteration 1 --
float(1.570796327)

-- Iteration 2 --
float(1.527345431)

-- Iteration 3 --
float(1.089909047)

-- Iteration 4 --
float(2.051683607)

-- Iteration 5 --
float(1.071020961E-8)

-- Iteration 6 --
float(1.142532174)

-- Iteration 7 --
float(1.99906048)

-- Iteration 8 --
float(0.1841885846)

-- Iteration 9 --
float(1.570259559)

-- Iteration 10 --
float(1.54906062)

-- Iteration 11 --
float(1.570796327)

-- Iteration 12 --
float(1.570796327)

-- Iteration 13 --
float(1.527345431)

-- Iteration 14 --
float(1.570796327)

-- Iteration 15 --
float(1.527345431)

-- Iteration 16 --
float(1.570796327)

-- Iteration 17 --

Warning: atan2() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: atan2() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: atan2() expects parameter 2 to be float, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: atan2() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: atan2() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: atan2() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: atan2() expects parameter 2 to be float, object given in %s on line %d
NULL

-- Iteration 24 --
float(1.570796327)

-- Iteration 25 --
float(1.570796327)

-- Iteration 26 --

Warning: atan2() expects parameter 2 to be float, resource given in %s on line %d
NULL
===Done===

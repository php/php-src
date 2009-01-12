--TEST--
Test log() function : usage variations - different data types as $arg argument
--FILE--
<?php
/* Prototype  : float log  ( float $arg  [, float $base  ] )
 * Description: Natural logarithm.
 * Source code: ext/standard/math.c
 */

echo "*** Testing log() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of log()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(log($input, 10));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing log() : usage variations ***

-- Iteration 1 --
float(-INF)

-- Iteration 2 --
float(0)

-- Iteration 3 --
float(4.091491094268)

-- Iteration 4 --
float(NAN)

-- Iteration 5 --
float(9.3319298653812)

-- Iteration 6 --
float(1.0211892990699)

-- Iteration 7 --
float(NAN)

-- Iteration 8 --
float(11.091514977169)

-- Iteration 9 --
float(-8.9084850228307)

-- Iteration 10 --
float(-0.30102999566398)

-- Iteration 11 --
float(-INF)

-- Iteration 12 --
float(-INF)

-- Iteration 13 --
float(0)

-- Iteration 14 --
float(-INF)

-- Iteration 15 --
float(0)

-- Iteration 16 --
float(-INF)

-- Iteration 17 --

Warning: log() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: log() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: log() expects parameter 1 to be double, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: log() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: log() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: log() expects parameter 1 to be double, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: log() expects parameter 1 to be double, object given in %s on line %d
NULL

-- Iteration 24 --
float(-INF)

-- Iteration 25 --
float(-INF)

-- Iteration 26 --

Warning: log() expects parameter 1 to be double, resource given in %s on line %d
NULL
===Done===
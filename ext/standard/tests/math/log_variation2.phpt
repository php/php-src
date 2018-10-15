--TEST--
Test log() function : usage variations - different data types as $base argument
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
	var_dump(log(3.14, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing log() : usage variations ***

-- Iteration 1 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 2 --
float(NAN)

-- Iteration 3 --
float(0.12145441273706)

-- Iteration 4 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 5 --
float(0.053250469650086)

-- Iteration 6 --
float(0.48661854224853)

-- Iteration 7 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 8 --
float(0.044802684673473)

-- Iteration 9 --
float(-0.055781611216686)

-- Iteration 10 --
float(-1.6507645591169)

-- Iteration 11 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 12 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 13 --
float(NAN)

-- Iteration 14 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 15 --
float(NAN)

-- Iteration 16 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 17 --

Warning: log() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: log() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: log() expects parameter 2 to be float, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: log() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: log() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: log() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: log() expects parameter 2 to be float, object given in %s on line %d
NULL

-- Iteration 24 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 25 --

Warning: log(): base must be greater than 0 in %s on line %d
bool(false)

-- Iteration 26 --

Warning: log() expects parameter 2 to be float, resource given in %s on line %d
NULL
===Done===

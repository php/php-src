--TEST--
Test fmod() function : usage variations - different data types as $y argument
--FILE--
<?php
/* Prototype  : float fmod  ( float $x  , float $y  )
 * Description: Returns the floating point remainder (modulo) of the division of the arguments.
 * Source code: ext/standard/math.c
 */

echo "*** Testing fmod() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of fmod()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(fmod(123456, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing fmod() : usage variations ***

-- Iteration 1 --
float(NAN)

-- Iteration 2 --
float(0)

-- Iteration 3 --
float(6)

-- Iteration 4 --
float(1516)

-- Iteration 5 --
float(123456)

-- Iteration 6 --
float(7.5)

-- Iteration 7 --
float(7.5)

-- Iteration 8 --
float(123456)

-- Iteration 9 --
float(2.3605615109341E-10)

-- Iteration 10 --
float(0)

-- Iteration 11 --
float(NAN)

-- Iteration 12 --
float(NAN)

-- Iteration 13 --
float(0)

-- Iteration 14 --
float(NAN)

-- Iteration 15 --
float(0)

-- Iteration 16 --
float(NAN)

-- Iteration 17 --

Warning: fmod() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: fmod() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: fmod() expects parameter 2 to be float, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: fmod() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: fmod() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: fmod() expects parameter 2 to be float, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: fmod() expects parameter 2 to be float, object given in %s on line %d
NULL

-- Iteration 24 --
float(NAN)

-- Iteration 25 --
float(NAN)

-- Iteration 26 --

Warning: fmod() expects parameter 2 to be float, resource given in %s on line %d
NULL
===Done===

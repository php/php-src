--TEST--
Test is_finite() function : usage variations - different data types as $val argument
--FILE--
<?php
/* Prototype  : bool is_finite  ( float $val  )
 * Description: Finds whether a value is a legal finite number.
 * Source code: ext/standard/math.c
 */

echo "*** Testing is_finite() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of is_finite()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(is_finite($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing is_finite() : usage variations ***

-- Iteration 1 --
bool(true)

-- Iteration 2 --
bool(true)

-- Iteration 3 --
bool(true)

-- Iteration 4 --
bool(true)

-- Iteration 5 --
bool(true)

-- Iteration 6 --
bool(true)

-- Iteration 7 --
bool(true)

-- Iteration 8 --
bool(true)

-- Iteration 9 --
bool(true)

-- Iteration 10 --
bool(true)

-- Iteration 11 --
bool(true)

-- Iteration 12 --
bool(true)

-- Iteration 13 --
bool(true)

-- Iteration 14 --
bool(true)

-- Iteration 15 --
bool(true)

-- Iteration 16 --
bool(true)

-- Iteration 17 --

Warning: is_finite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: is_finite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: is_finite() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: is_finite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: is_finite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: is_finite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: is_finite() expects parameter 1 to be float, object given in %s on line %d
NULL

-- Iteration 24 --
bool(true)

-- Iteration 25 --
bool(true)

-- Iteration 26 --

Warning: is_finite() expects parameter 1 to be float, resource given in %s on line %d
NULL
===Done===

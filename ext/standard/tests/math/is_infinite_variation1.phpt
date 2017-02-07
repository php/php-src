--TEST--
Test is_infinite() function : usage variations - different data types as $val argument
--FILE--
<?php
/* Prototype  : bool is_finite  ( float $val  )
 * Description: Finds whether a value is infinite.
 * Source code: ext/standard/math.c
 */

echo "*** Testing is_infinite() : usage variations ***\n";

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
       NAN,
       INF,
       -INF,

       // null data
/*14*/ NULL,
       null,

       // boolean data
/*16*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*20*/ "",
       '',
       array(),

       // string data
/*23*/ "abcxyz",
       'abcxyz',
       $heredoc,
       
       // object data
/*26*/ new classA(),       
       
       // undefined data
/*27*/ @$undefined_var,

       // unset data
/*28*/ @$unset_var,

       // resource variable
/*29*/ $fp
);

// loop through each element of $inputs to check the behaviour of is_infinite()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(is_infinite($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing is_infinite() : usage variations ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(false)

-- Iteration 12 --
bool(true)

-- Iteration 13 --
bool(true)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --
bool(false)

-- Iteration 19 --
bool(false)

-- Iteration 20 --

Warning: is_infinite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: is_infinite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: is_infinite() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 23 --

Warning: is_infinite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 24 --

Warning: is_infinite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 25 --

Warning: is_infinite() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 26 --

Warning: is_infinite() expects parameter 1 to be float, object given in %s on line %d
NULL

-- Iteration 27 --
bool(false)

-- Iteration 28 --
bool(false)

-- Iteration 29 --

Warning: is_infinite() expects parameter 1 to be float, resource given in %s on line %d
NULL
===Done===

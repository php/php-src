--TEST--
Test base_convert() function : usage variations - different data types as $frombase argument
--FILE--
<?php
/* Prototype  : string base_convert  ( string $number  , int $frombase  , int $tobase  )
 * Description: Convert a number between arbitrary bases.
 * Source code: ext/standard/math.c
 */

echo "*** Testing base_convert() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

$inputs = array(
       // int data
/*1*/  0,
       1,
       -1,
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
       
       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,
);

// loop through each element of $inputs to check the behaviour of base_convert()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(base_convert(25, $input, 8));
	$iterator++;
};
?>
===Done===
--EXPECTF--
*** Testing base_convert() : usage variations ***

-- Iteration 1 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 2 --

Warning: base_convert(): Invalid `from base' (1) in %s on line %d
bool(false)

-- Iteration 3 --

Warning: base_convert(): Invalid `from base' (-1) in %s on line %d
bool(false)

-- Iteration 4 --

Warning: base_convert(): Invalid `from base' (-12) in %s on line %d
bool(false)

-- Iteration 5 --

Warning: base_convert(): Invalid `from base' (2147483647) in %s on line %d
bool(false)

-- Iteration 6 --
string(2) "31"

-- Iteration 7 --

Warning: base_convert(): Invalid `from base' (-10) in %s on line %d
bool(false)

-- Iteration 8 --

Warning: base_convert(): Invalid `from base' (123) in %s on line %d
bool(false)

-- Iteration 9 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 10 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 11 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 12 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 13 --

Warning: base_convert(): Invalid `from base' (1) in %s on line %d
bool(false)

-- Iteration 14 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 15 --

Warning: base_convert(): Invalid `from base' (1) in %s on line %d
bool(false)

-- Iteration 16 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 17 --

Warning: base_convert() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: base_convert() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: base_convert() expects parameter 2 to be long, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: base_convert() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: base_convert() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: base_convert() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)

-- Iteration 24 --

Warning: base_convert(): Invalid `from base' (0) in %s on line %d
bool(false)
===Done===
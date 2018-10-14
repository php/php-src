--TEST--
Test srand() function : usage variations - different data types as $seed argument
--SKIPIF--
<?php if (PHP_INT_SIZE !== 4) die("skip this test is for 32-bit only");
--FILE--
<?php
/* Prototype  : void srand  ([ int $seed  ] )
 * Description: Seed the random number generator.
 * Source code: ext/standard/rand.c
 */

echo "*** Testing srand() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of srand()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(srand($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing srand() : usage variations ***

-- Iteration 1 --
NULL

-- Iteration 2 --
NULL

-- Iteration 3 --
NULL

-- Iteration 4 --
NULL

-- Iteration 5 --
NULL

-- Iteration 6 --
NULL

-- Iteration 7 --
NULL

-- Iteration 8 --

Warning: srand() expects parameter 1 to be int, float given in %s on line %d
NULL

-- Iteration 9 --
NULL

-- Iteration 10 --
NULL

-- Iteration 11 --
NULL

-- Iteration 12 --
NULL

-- Iteration 13 --
NULL

-- Iteration 14 --
NULL

-- Iteration 15 --
NULL

-- Iteration 16 --
NULL

-- Iteration 17 --

Warning: srand() expects parameter 1 to be int, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: srand() expects parameter 1 to be int, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: srand() expects parameter 1 to be int, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: srand() expects parameter 1 to be int, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: srand() expects parameter 1 to be int, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: srand() expects parameter 1 to be int, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: srand() expects parameter 1 to be int, object given in %s on line %d
NULL

-- Iteration 24 --
NULL

-- Iteration 25 --
NULL

-- Iteration 26 --

Warning: srand() expects parameter 1 to be int, resource given in %s on line %d
NULL
===Done===

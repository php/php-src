--TEST--
Test rand() function : usage variations - different data types as $max argument
--SKIPIF--
<?php if (PHP_INT_SIZE !== 4) die("skip this test is for 32-bit only");
--FILE--
<?php
/* Prototype  : int rand  ([ int $min  , int $max ] )
 * Description: Generate a random integer.
 * Source code: ext/standard/rand.c
 */

echo "*** Testing rand) : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of rand()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(rand(100, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing rand) : usage variations ***

-- Iteration 1 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 2 --

Warning: rand(): max(1) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 3 --
int(%i)

-- Iteration 4 --

Warning: rand(): max(-2345) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 5 --
int(%i)

-- Iteration 6 --

Warning: rand(): max(10) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 7 --

Warning: rand(): max(-10) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 8 --

Warning: rand() expects parameter 2 to be integer, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 10 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 11 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 12 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 13 --

Warning: rand(): max(1) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 14 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 15 --

Warning: rand(): max(1) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 16 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 17 --

Warning: rand() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: rand() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: rand() expects parameter 2 to be integer, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: rand() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: rand() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: rand() expects parameter 2 to be integer, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: rand() expects parameter 2 to be integer, object given in %s on line %d
NULL

-- Iteration 24 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 25 --

Warning: rand(): max(0) is smaller than min(100) in %s on line %d
bool(false)

-- Iteration 26 --

Warning: rand() expects parameter 2 to be integer, resource given in %s on line %d
NULL
===Done===

--TEST--
Test mt_rand() function : usage variations - different data types as $max argument
--FILE--
<?php
/* Prototype  : int mt_rand  ([ int $min  , int $max ] )
 * Description: Generate a better random value.
 * Source code: ext/standard/rand.c
 */

echo "*** Testing mt_rand) : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of mt_rand()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(mt_rand(-1 * mt_getrandmax(), $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing mt_rand) : usage variations ***

-- Iteration 1 --
int(%i)

-- Iteration 2 --
int(%i)

-- Iteration 3 --
int(%i)

-- Iteration 4 --
int(%i)

-- Iteration 5 --
int(%i)

-- Iteration 6 --
int(%i)

-- Iteration 7 --
int(%i)

-- Iteration 8 --
int(%i)

-- Iteration 9 --
int(%i)

-- Iteration 10 --
int(%i)

-- Iteration 11 --
int(%i)

-- Iteration 12 --
int(%i)

-- Iteration 13 --
int(%i)

-- Iteration 14 --
int(%i)

-- Iteration 15 --
int(%i)

-- Iteration 16 --
int(%i)

-- Iteration 17 --

Warning: mt_rand() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: mt_rand() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: mt_rand() expects parameter 2 to be long, array given in %s on line %d
NULL

-- Iteration 20 --

Warning: mt_rand() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: mt_rand() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: mt_rand() expects parameter 2 to be long, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: mt_rand() expects parameter 2 to be long, object given in %s on line %d
NULL

-- Iteration 24 --
int(%i)

-- Iteration 25 --
int(%i)

-- Iteration 26 --

Warning: mt_rand() expects parameter 2 to be long, resource given in %s on line %d
NULL
===Done===

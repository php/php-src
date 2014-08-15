--TEST--
Test expm1() function : usage variations - different data types as $arg argument
--INI--
precision=14
--FILE--
<?php

/* Prototype  : float expm1  ( float $arg  )
 * Description: Returns exp(number) - 1, computed in a way that is accurate even 
 *              when the value of number is close to zero.
 * Source code: ext/standard/math.c
 */

echo "*** Testing expm1() : usage variations ***\n";

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

// unexpected values to be passed to $arg argument
$inputs = array(
       // int data
/*1*/  0,
       1,
       12345,
       -2345,       

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789E4,
       12.3456789E-4,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "abcxyz",
       'abcxyz',
       $heredoc,
       
       // array data
       array(),
       array(1,2,4),
       
       // object data
/*24*/ new classA(),       
       
       // undefined data
/*25*/ @$undefined_var,

       // unset data
/*26*/ @$unset_var,

       // resource variable
/*27*/ $fp
);

// loop through each element of $inputs to check the behaviour of expm1()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(expm1($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing expm1() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(1.718281828459)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(-1)

-- Iteration 5 --
float(36314.502674247)

-- Iteration 6 --
float(-0.99997246355065)

-- Iteration 7 --
float(INF)

-- Iteration 8 --
float(0.0012353302826471)

-- Iteration 9 --
float(0.64872127070013)

-- Iteration 10 --
float(0)

-- Iteration 11 --
float(0)

-- Iteration 12 --
float(1.718281828459)

-- Iteration 13 --
float(0)

-- Iteration 14 --
float(1.718281828459)

-- Iteration 15 --
float(0)

-- Iteration 16 --

Warning: expm1() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: expm1() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: expm1() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 19 --

Warning: expm1() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: expm1() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: expm1() expects parameter 1 to be float, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: expm1() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 23 --

Warning: expm1() expects parameter 1 to be float, array given in %s on line %d
NULL

-- Iteration 24 --

Warning: expm1() expects parameter 1 to be float, object given in %s on line %d
NULL

-- Iteration 25 --
float(0)

-- Iteration 26 --
float(0)

-- Iteration 27 --

Warning: expm1() expects parameter 1 to be float, resource given in %s on line %d
NULL
===Done===
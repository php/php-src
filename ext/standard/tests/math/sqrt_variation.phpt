--TEST--
Test variations in usage of sqrt()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float sqrt(float number)
 * Function is implemented in ext/standard/math.c
*/ 


echo "*** Testing sqrt() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of sqrt()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(sqrt($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing sqrt() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(1)

-- Iteration 3 --
float(111.10805551354)

-- Iteration 4 --
float(NAN)

-- Iteration 5 --
float(46340.950001052)

-- Iteration 6 --
float(3.2403703492039)

-- Iteration 7 --
float(NAN)

-- Iteration 8 --
float(351364.18286445)

-- Iteration 9 --
float(3.5136418286445E-5)

-- Iteration 10 --
float(0.70710678118655)

-- Iteration 11 --
float(0)

-- Iteration 12 --
float(0)

-- Iteration 13 --
float(1)

-- Iteration 14 --
float(0)

-- Iteration 15 --
float(1)

-- Iteration 16 --
float(0)

-- Iteration 17 --
float(0)

-- Iteration 18 --
float(0)

-- Iteration 19 --
float(0)

-- Iteration 20 --
float(0)

-- Iteration 21 --
float(0)

-- Iteration 22 --
float(0)

-- Iteration 23 --

Notice: Object of class classA could not be converted to double in %s on line %d
float(1)

-- Iteration 24 --
float(0)

-- Iteration 25 --
float(0)

-- Iteration 26 --
float(%f)
===Done===


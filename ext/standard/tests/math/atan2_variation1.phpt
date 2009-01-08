--TEST--
Test atan2() function : usage variations - different data types as $y arg
--INI--
precision = 10
--FILE--
<?php
/* Prototype  : float atan2  ( float $y  , float $x  )
 * Description: Arc tangent of two variables.
 * Source code: ext/standard/math.c
 */

echo "*** Testing atan2() : usage variations ***\n";

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
       12,
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
       
       // object data
/*23*/ new classA(),       
       
       // undefined data
/*24*/ @$undefined_var,

       // unset data
/*25*/ @$unset_var,

       // resource variable
/*26*/ $fp
);

// loop through each element of $inputs to check the behaviour of atan2()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(atan2($input, 23));
	$iterator++;
};

fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing atan2() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(0.04345089539)

-- Iteration 3 --
float(0.4808872802)

-- Iteration 4 --
float(-0.4808872802)

-- Iteration 5 --
float(1.570796316)

-- Iteration 6 --
float(0.4282641529)

-- Iteration 7 --
float(-0.4282641529)

-- Iteration 8 --
float(1.386607742)

-- Iteration 9 --
float(0.0005367682093)

-- Iteration 10 --
float(0.02173570684)

-- Iteration 11 --
float(0)

-- Iteration 12 --
float(0)

-- Iteration 13 --
float(0.04345089539)

-- Iteration 14 --
float(0)

-- Iteration 15 --
float(0.04345089539)

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
float(0.04345089539)

-- Iteration 24 --
float(0)

-- Iteration 25 --
float(0)

-- Iteration 26 --
float(%f)
===Done===
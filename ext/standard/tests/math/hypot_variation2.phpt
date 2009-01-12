--TEST--
Test hypot() function : usage variations - different data types as $y argument
--FILE--
<?php
/* Prototype  : float hypot  ( float $x  , float $y  )
 * Description: Calculate the length of the hypotenuse of a right-angle triangle
 * Source code: ext/standard/math.c
 */

echo "*** Testing hypot() : usage variations ***\n";

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

// loop through each element of $inputs to check the behaviour of hypot()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(hypot(3, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing hypot() : usage variations ***

-- Iteration 1 --
float(3)

-- Iteration 2 --
float(3.1622776601684)

-- Iteration 3 --
float(12345.00036452)

-- Iteration 4 --
float(2345.0019189758)

-- Iteration 5 --
float(2147483647)

-- Iteration 6 --
float(10.920164833921)

-- Iteration 7 --
float(10.920164833921)

-- Iteration 8 --
float(123456789000)

-- Iteration 9 --
float(3)

-- Iteration 10 --
float(3.0413812651491)

-- Iteration 11 --
float(3)

-- Iteration 12 --
float(3)

-- Iteration 13 --
float(3.1622776601684)

-- Iteration 14 --
float(3)

-- Iteration 15 --
float(3.1622776601684)

-- Iteration 16 --
float(3)

-- Iteration 17 --
float(3)

-- Iteration 18 --
float(3)

-- Iteration 19 --
float(3)

-- Iteration 20 --
float(3)

-- Iteration 21 --
float(3)

-- Iteration 22 --
float(3)

-- Iteration 23 --

Notice: Object of class classA could not be converted to double in %s on line %d
float(3.1622776601684)

-- Iteration 24 --
float(3)

-- Iteration 25 --
float(3)

-- Iteration 26 --
float(%f)
===Done===

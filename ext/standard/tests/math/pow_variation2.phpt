--TEST--
Test pow() function : usage variations - different data types as $exp argument
--INI--
precision = 14
--FILE--
<?php
/* Prototype  : number pow  ( number $base  , number $exp  )
 * Description: Exponential expression.
 * Source code: ext/standard/math.c
 */

echo "*** Testing pow() : usage variations ***\n";

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
/*6*/  2.5,
       -2.5,
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

// loop through each element of $inputs to check the behaviour of pow()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(pow(20.3, $input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing pow() : usage variations ***

-- Iteration 1 --
float(1)

-- Iteration 2 --
float(20.3)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(INF)

-- Iteration 6 --
float(1856.6929774279)

-- Iteration 7 --
float(0.00053859200856424)

-- Iteration 8 --
float(INF)

-- Iteration 9 --
float(1.0000000037168)

-- Iteration 10 --
float(4.5055521304275)

-- Iteration 11 --
float(1)

-- Iteration 12 --
float(1)

-- Iteration 13 --
float(20.3)

-- Iteration 14 --
float(1)

-- Iteration 15 --
float(20.3)

-- Iteration 16 --
float(1)

-- Iteration 17 --

Warning: A non-numeric value encountered in %s on line %d
float(1)

-- Iteration 18 --

Warning: A non-numeric value encountered in %s on line %d
float(1)

-- Iteration 19 --
int(1)

-- Iteration 20 --

Warning: A non-numeric value encountered in %s on line %d
float(1)

-- Iteration 21 --

Warning: A non-numeric value encountered in %s on line %d
float(1)

-- Iteration 22 --

Warning: A non-numeric value encountered in %s on line %d
float(1)

-- Iteration 23 --

Notice: Object of class classA could not be converted to number in %s on line %d
float(20.3)

-- Iteration 24 --
float(1)

-- Iteration 25 --
float(1)

-- Iteration 26 --
%s
===Done===

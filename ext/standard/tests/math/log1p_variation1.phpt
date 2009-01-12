--TEST--
Test log1p() function : usage variations - different data types as $arg argument
--INI--
precision=14
--SKIPIF--
<?php
	function_exists('log1p') or die('skip log1p() is not supported in this build.');
?>
--FILE--
<?php
/* Prototype  : float log1p  ( float $arg  )
 * Description: Returns log(1 + number), computed in a way that is accurate even 
 *				when the value of number is close to zero
 * Source code: ext/standard/math.c
 */

echo "*** Testing log1p() : usage variations ***\n";

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
       -2147483648,

       // float data
/*7*/  10.5,
       -10.5,
       12.3456789E4,
       12.3456789E-4,
       .5,

       // null data
/*12*/ NULL,
       null,

       // boolean data
/*14*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*18*/ "",
       '',
       array(),

       // string data
/*21*/ "abcxyz",
       'abcxyz',
       $heredoc,
       
       // object data
/*24*/ new classA(),       
       
       // undefined data
/*25*/ @$undefined_var,

       // unset data
/*26*/ @$unset_var,

       // resource variable
/*27*/ $fp
);

// loop through each element of $inputs to check the behaviour of log1p()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(log1p($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing log1p() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(0.69314718055995)

-- Iteration 3 --
float(9.4210874029538)

-- Iteration 4 --
float(NAN)

-- Iteration 5 --
float(21.487562597358)

-- Iteration 6 --
float(NAN)

-- Iteration 7 --
float(2.4423470353692)

-- Iteration 8 --
float(NAN)

-- Iteration 9 --
float(11.723654587153)

-- Iteration 10 --
float(0.0012338064377078)

-- Iteration 11 --
float(0.40546510810816)

-- Iteration 12 --
float(0)

-- Iteration 13 --
float(0)

-- Iteration 14 --
float(0.69314718055995)

-- Iteration 15 --
float(0)

-- Iteration 16 --
float(0.69314718055995)

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
float(0)

-- Iteration 24 --

Notice: Object of class classA could not be converted to double in %s on line %d
float(0.69314718055995)

-- Iteration 25 --
float(0)

-- Iteration 26 --
float(0)

-- Iteration 27 --
float(%f)
===Done===

--TEST--
Test bindec() function : usage variations - different data types as $binary_string arg
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : number bindec  ( string $binary_string  )
 * Description: Returns the decimal equivalent of the binary number represented by the binary_string  argument.
 * Source code: ext/standard/math.c
 */

echo "*** Testing bindec() : usage variations ***\n";
//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = array(
       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
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

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behaviour of bindec()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(bindec($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing bindec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(1)

-- Iteration 4 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 5 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(2)

-- Iteration 6 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(2)

-- Iteration 7 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(8)

-- Iteration 8 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(1)

-- Iteration 9 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 10 --
int(0)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(1)

-- Iteration 13 --
int(0)

-- Iteration 14 --
int(1)

-- Iteration 15 --
int(0)

-- Iteration 16 --
int(0)

-- Iteration 17 --
int(0)

-- Iteration 18 --

Notice: Array to string conversion in %s on line %d

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 19 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 20 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 21 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(0)

-- Iteration 24 --

Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(0)
===Done===

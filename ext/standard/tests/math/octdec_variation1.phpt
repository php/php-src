--TEST--
Test octdec() function : usage variations - different data types as $octal_string arg
--INI--
precision=14
--FILE--
<?php
/* Prototype  : number octdec  ( string $octal_string  )
 * Description: Returns the decimal equivalent of the octal number represented by the octal_string  argument.
 * Source code: ext/standard/math.c
 */

echo "*** Testing octdec() : usage variations ***\n";
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
       4294967295,  // largest decimal
       4294967296,

       // float data
/*7*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
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

       // undefined data
/*24*/ @$undefined_var,

       // unset data
/*25*/ @$unset_var,

       // resource variable
/*26*/ $fp
);

// loop through each element of $inputs to check the behaviour of octdec()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(octdec($input));
	$iterator++;
};
fclose($fp);
?>
---Done---
--EXPECTF--
*** Testing octdec() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(5349)

-- Iteration 4 --
int(1253)

-- Iteration 5 --
int(1134037)

-- Iteration 6 --
int(1134038)

-- Iteration 7 --
int(69)

-- Iteration 8 --
int(69)

-- Iteration 9 --
int(175304192)

-- Iteration 10 --
int(342391)

-- Iteration 11 --
int(5)

-- Iteration 12 --
int(0)

-- Iteration 13 --
int(0)

-- Iteration 14 --
int(1)

-- Iteration 15 --
int(0)

-- Iteration 16 --
int(1)

-- Iteration 17 --
int(0)

-- Iteration 18 --
int(0)

-- Iteration 19 --
int(0)

-- Iteration 20 --

Notice: Array to string conversion in %s on line %d
int(0)

-- Iteration 21 --
int(0)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(0)

-- Iteration 24 --
int(0)

-- Iteration 25 --
int(0)

-- Iteration 26 --
int(%d)
---Done---

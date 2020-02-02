--TEST--
Test base_convert() function : usage variations - different data types as $number argument
--FILE--
<?php
/* Prototype  : string base_convert  ( string $number  , int $frombase  , int $tobase  )
 * Description: Convert a number between arbitrary bases.
 * Source code: ext/standard/math.c
 */

echo "*** Testing base_convert() : usage variations ***\n";

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

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behaviour of base_convert()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
    try {
        $output = base_convert($input, 10, 8);
    } catch (InvalidArgumentException $e) {
        $output = 'INVALID';
    }
	var_dump($output);
	$iterator++;
};
fclose($fp);
?>
--EXPECTF--
*** Testing base_convert() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(2) "14"

-- Iteration 4 --
string(7) "INVALID"

-- Iteration 5 --
string(11) "17777777777"

-- Iteration 6 --
string(7) "INVALID"

-- Iteration 7 --
string(7) "INVALID"

-- Iteration 8 --
string(7) "INVALID"

-- Iteration 9 --
string(7) "INVALID"

-- Iteration 10 --
string(7) "INVALID"

-- Iteration 11 --
string(1) "0"

-- Iteration 12 --
string(1) "0"

-- Iteration 13 --
string(1) "1"

-- Iteration 14 --
string(1) "0"

-- Iteration 15 --
string(1) "1"

-- Iteration 16 --
string(1) "0"

-- Iteration 17 --
string(1) "0"

-- Iteration 18 --
string(1) "0"

-- Iteration 19 --

Warning: Array to string conversion in %s on line %d
string(7) "INVALID"

-- Iteration 20 --
string(7) "INVALID"

-- Iteration 21 --
string(7) "INVALID"

-- Iteration 22 --
string(7) "INVALID"

-- Iteration 23 --
string(1) "0"

-- Iteration 24 --
string(1) "0"

-- Iteration 25 --
string(7) "INVALID"

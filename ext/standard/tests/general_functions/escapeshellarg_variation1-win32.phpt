--TEST--
Test escapeshellarg() function : usage variations - different data types as $arg arg
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. only for Windows");
?>
--FILE--
<?php

/* Prototype  : string escapeshellarg  ( string $arg  )
 * Description:  Escape a string to be used as a shell argument.
 * Source code: ext/standard/exec.c
 */

echo "*** Testing escapeshellarg() : usage variations ***\n";

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

       // undefined data
/*19*/ @$undefined_var,

       // unset data
/*20*/ @$unset_var,

);

// loop through each element of $inputs to check the behaviour of escapeshellarg()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(escapeshellarg($input));
	$iterator++;
};
?>
===Done===
--EXPECT--
*** Testing escapeshellarg() : usage variations ***

-- Iteration 1 --
string(3) ""0""

-- Iteration 2 --
string(3) ""1""

-- Iteration 3 --
string(4) ""12""

-- Iteration 4 --
string(5) ""-12""

-- Iteration 5 --
string(12) ""2147483647""

-- Iteration 6 --
string(6) ""10.5""

-- Iteration 7 --
string(7) ""-10.5""

-- Iteration 8 --
string(10) ""123.4567""

-- Iteration 9 --
string(12) ""0.01234567""

-- Iteration 10 --
string(5) ""0.5""

-- Iteration 11 --
string(2) """"

-- Iteration 12 --
string(2) """"

-- Iteration 13 --
string(3) ""1""

-- Iteration 14 --
string(2) """"

-- Iteration 15 --
string(3) ""1""

-- Iteration 16 --
string(2) """"

-- Iteration 17 --
string(2) """"

-- Iteration 18 --
string(2) """"

-- Iteration 19 --
string(2) """"

-- Iteration 20 --
string(2) """"
===Done===

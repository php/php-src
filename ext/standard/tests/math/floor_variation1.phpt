--TEST--
Test floor() function : usage variations - different data types as $value arg
--INI--
precision=14
--FILE--
<?php
/* Prototype  : float floor  ( float $value  )
 * Description: Round fractions down.
 * Source code: ext/standard/math.c
 */

echo "*** Testing floor() : usage variations ***\n";
//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
}

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $value argument
$inputs = array(
       // null data
/* 1*/ NULL,
       null,

       // boolean data
/* 3*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/* 7*/ "",
       '',
       array(),

       // string data
/*10*/ "abcxyz",
       'abcxyz}',
       $heredoc,

       // object data
/*13*/ new classA(),

       // undefined data
/*14*/ @$undefined_var,

       // unset data
/*15*/ @$unset_var,

       // resource variable
/*16*/ $fp
);

// loop through each element of $inputs to check the behaviour of floor()
$iterator = 1;
foreach($inputs as $input) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(floor($input));
	$iterator++;
};
fclose($fp);
?>
===Done===
--EXPECTF--
*** Testing floor() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
float(0)

-- Iteration 3 --
float(1)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(1)

-- Iteration 6 --
float(0)

-- Iteration 7 --
float(0)

-- Iteration 8 --
float(0)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
float(0)

-- Iteration 11 --
float(0)

-- Iteration 12 --
float(0)

-- Iteration 13 --

Notice: Object of class classA could not be converted to int in %s on line %d
float(1)

-- Iteration 14 --
float(0)

-- Iteration 15 --
float(0)

-- Iteration 16 --
float(%f)
===Done===

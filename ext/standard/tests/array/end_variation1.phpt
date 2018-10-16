--TEST--
Test end() function : usage variations - Pass different data types as $array_arg
--FILE--
<?php
/* Prototype  : mixed end(array $array_arg)
 * Description: Advances array argument's internal pointer to the last element and return it
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $array_arg to test behaviour of end()
 */

echo "*** Testing end() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
	var $foo = 'hello, world';
	public function __toString() {
		return "Class A object";
	}
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $array_arg argument
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
/*19*/ "string",
       'string',
       $heredoc,

       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of end()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( end($input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing end() : usage variations ***

-- Iteration 1 --

Warning: end() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 2 --

Warning: end() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 3 --

Warning: end() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 4 --

Warning: end() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Iteration 5 --

Warning: end() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: end() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: end() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: end() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: end() expects parameter 1 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: end() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: end() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: end() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 13 --

Warning: end() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 14 --

Warning: end() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 15 --

Warning: end() expects parameter 1 to be array, bool given in %s on line %d
NULL

-- Iteration 16 --

Warning: end() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: end() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 18 --
bool(false)

-- Iteration 19 --

Warning: end() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: end() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: end() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Iteration 22 --
string(12) "hello, world"

-- Iteration 23 --

Warning: end() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 24 --

Warning: end() expects parameter 1 to be array, null given in %s on line %d
NULL

-- Iteration 25 --

Warning: end() expects parameter 1 to be array, resource given in %s on line %d
NULL
===DONE===

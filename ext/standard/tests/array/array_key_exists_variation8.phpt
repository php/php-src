--TEST--
Test array_key_exists() function : usage variations - array keys are different data types
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Pass an array where the keys are different data types as the $search argument
 * then pass many different data types as $key argument to test where array_key_exist()
 * returns true.
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
string
EOT;

// different data types to be iterated over
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // float data
/*2*/  'float' => array(
       10.5 => 'positive',
       -10.5 => 'negative',
       .5 => 'half',
       ),

       'extreme floats' => array(
       12.3456789000e10 => 'large',
       12.3456789000E-10 => 'small',
       ),

       // null data
/*3*/ 'null uppercase' => array(
       NULL => 'null 1',
       ),
       'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*4*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),
       'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),

       // empty data
/*5*/ 'empty double quotes' => array(
       "" => 'emptyd',
       ),
       'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*6*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // undefined data
/*8*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*9*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each element of $inputs to check the behavior of array_key_exists()
$iterator = 1;
foreach($inputs as $type => $input) {
	echo "\n-- Iteration $iterator: $type data --\n";

	//iterate over again to get all different key values
	foreach ($inputs as $new_type => $new_input) {
		echo "-- \$key arguments are $new_type data:\n";
		foreach ($new_input as $key => $search) {
			var_dump(array_key_exists($key, $input));
		}
	}
	$iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_key_exists() : usage variations ***

-- Iteration 1: int data --
-- $key arguments are int data:
bool(true)
bool(true)
bool(true)
bool(true)
-- $key arguments are float data:
bool(false)
bool(false)
bool(true)
-- $key arguments are extreme floats data:
bool(false)
bool(true)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(true)
bool(true)
-- $key arguments are bool uppercase data:
bool(true)
bool(true)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 2: float data --
-- $key arguments are int data:
bool(true)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(true)
bool(true)
bool(true)
-- $key arguments are extreme floats data:
bool(false)
bool(true)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(false)
bool(true)
-- $key arguments are bool uppercase data:
bool(false)
bool(true)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 3: extreme floats data --
-- $key arguments are int data:
bool(true)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(true)
-- $key arguments are extreme floats data:
bool(true)
bool(true)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(false)
bool(true)
-- $key arguments are bool uppercase data:
bool(false)
bool(true)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 4: null uppercase data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)

-- Iteration 5: null lowercase data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)

-- Iteration 6: bool lowercase data --
-- $key arguments are int data:
bool(true)
bool(true)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(true)
-- $key arguments are extreme floats data:
bool(false)
bool(true)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(true)
bool(true)
-- $key arguments are bool uppercase data:
bool(true)
bool(true)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 7: bool uppercase data --
-- $key arguments are int data:
bool(true)
bool(true)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(true)
-- $key arguments are extreme floats data:
bool(false)
bool(true)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(true)
bool(true)
-- $key arguments are bool uppercase data:
bool(true)
bool(true)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 8: empty double quotes data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)

-- Iteration 9: empty single quotes data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)

-- Iteration 10: string data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(false)
-- $key arguments are null lowercase data:
bool(false)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(true)
bool(true)
bool(true)
-- $key arguments are undefined data:
bool(false)
-- $key arguments are unset data:
bool(false)

-- Iteration 11: undefined data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)

-- Iteration 12: unset data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are float data:
bool(false)
bool(false)
bool(false)
-- $key arguments are extreme floats data:
bool(false)
bool(false)
-- $key arguments are null uppercase data:
bool(true)
-- $key arguments are null lowercase data:
bool(true)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are bool uppercase data:
bool(false)
bool(false)
-- $key arguments are empty double quotes data:
bool(true)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)
-- $key arguments are undefined data:
bool(true)
-- $key arguments are unset data:
bool(true)
Done

--TEST--
Test array_key_exists() function : usage variations - Pass different data types as $key arg
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Pass different data types as $key argument to array_key_exists() to test behaviour
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

// Initialise function arguments not being substituted
$search = array ('zero', 'key' => 'val', 'two');

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "key";
  }
}

// heredoc string
$heredoc = <<<EOT
key
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $key argument
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
/*19*/ "key",
       'key',
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

// loop through each element of $inputs to check the behavior of array_key_exists()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_key_exists($input, $search) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- Iteration 1 --
bool(true)

-- Iteration 2 --
bool(true)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 6 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 7 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 8 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 9 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(false)

-- Iteration 12 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 13 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 14 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 15 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 19 --
bool(true)

-- Iteration 20 --
bool(true)

-- Iteration 21 --
bool(true)

-- Iteration 22 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

-- Iteration 23 --
bool(false)

-- Iteration 24 --
bool(false)

-- Iteration 25 --

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)
Done

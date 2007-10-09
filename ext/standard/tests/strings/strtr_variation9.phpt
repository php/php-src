--TEST--
Test strtr() function : usage variations - unexpected inputs for all arguments
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'str', 'from', 'to' & 'replace_pairs' arguments */

echo "*** Testing strtr() function: with unexpected inputs for all arguments ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  } 
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values
$values =  array (

  // integer values
  0,
  1,
  -2,

  // float values
  10.5,
  -20.5,
  10.5e10,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // null vlaues
  NULL,
  null,

  // objects
  new sample(),

  // resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop through with each element of the $values array to test strtr() function
$count = 1;
for($index = 0; $index < count($values); $index++) {
  echo "\n-- Iteration $count --\n";
  var_dump( strtr($values[$index], $values[$index], $values[$index]) ); //fn call with three args
  var_dump( strtr($values[$index], $values[$index]) );  //fn call with two args
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for all arguments ***

-- Iteration 1 --
string(1) "0"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 2 --
string(1) "1"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 3 --
string(2) "-2"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 4 --
string(4) "10.5"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 5 --
string(5) "-20.5"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 6 --
string(12) "105000000000"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 7 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

-- Iteration 8 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

-- Iteration 9 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

-- Iteration 10 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

-- Iteration 11 --

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(5) "Array"

Notice: Array to string conversion in %s on line %d
string(5) "Array"

-- Iteration 12 --
string(1) "1"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 13 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 14 --
string(1) "1"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 15 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 16 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 17 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 18 --
string(13) "sample object"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 19 --
string(%d) "Resource id #%d"

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 20 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 21 --
string(0) ""

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)
*** Done ***

--TEST--
Test strtr() function : usage variations - unexpected inputs for 'replace_pairs' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'replace_pairs' 
 *  and expected type for 'str' arguments 
*/

echo "*** Testing strtr() function: with unexpected inputs for 'replace_pairs' ***\n";

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

//defining 'str' argument
$str = "012atm";

// array of inputs for 'replace_pairs' argument
$replace_pairs_arr =  array (

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
  array(1, 2),

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

// loop through with each element of the $replace_pairs array to test strtr() function
$count = 1;
for($index = 0; $index < count($replace_pairs_arr); $index++) {
  echo "\n-- Iteration $count --\n";
  $replace_pairs = $replace_pairs_arr[$index];
  var_dump( strtr($str, $replace_pairs) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for 'replace_pairs' ***

-- Iteration 1 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 2 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 3 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 4 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 5 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 6 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 7 --
string(6) "012atm"

-- Iteration 8 --
string(6) "012atm"

-- Iteration 9 --
string(6) "122atm"

-- Iteration 10 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 11 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 12 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 13 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 14 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 15 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 16 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 17 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 18 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)

-- Iteration 19 --

Warning: strtr(): The second argument is not an array in %s on line %d
bool(false)
*** Done ***

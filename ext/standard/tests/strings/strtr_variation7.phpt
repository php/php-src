--TEST--
Test strtr() function : usage variations - unexpected inputs for 'to' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'to' 
 *  and expected types for 'str' & 'from' arguments 
*/

echo "*** Testing strtr() function: with unexpected inputs for 'to' ***\n";

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

//defining 'from' argument
$from = "atm012";

// array of values for 'to' argument
$to_arr =  array (

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

// loop through with each element of the $to array to test strtr() function
$count = 1;
for($index = 0; $index < count($to_arr); $index++) {
  echo "\n-- Iteration $count --\n";
  $to = $to_arr[$index];
  var_dump( strtr($str, $from, $to) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for 'to' ***

-- Iteration 1 --
string(6) "0120tm"

-- Iteration 2 --
string(6) "0121tm"

-- Iteration 3 --
string(6) "012-2m"

-- Iteration 4 --
string(6) "51210."

-- Iteration 5 --
string(6) ".52-20"

-- Iteration 6 --
string(6) "000105"

-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
string(6) "ay2Arr"

-- Iteration 8 --

Notice: Array to string conversion in %s on line %d
string(6) "ay2Arr"

-- Iteration 9 --

Notice: Array to string conversion in %s on line %d
string(6) "ay2Arr"

-- Iteration 10 --
string(6) "0121tm"

-- Iteration 11 --
string(6) "012atm"

-- Iteration 12 --
string(6) "0121tm"

-- Iteration 13 --
string(6) "012atm"

-- Iteration 14 --
string(6) "012atm"

-- Iteration 15 --
string(6) "012atm"

-- Iteration 16 --
string(6) "plesam"

-- Iteration 17 --
string(6) "ourRes"

-- Iteration 18 --
string(6) "012atm"

-- Iteration 19 --
string(6) "012atm"
*** Done ***

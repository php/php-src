--TEST--
Test strtr() function : usage variations - unexpected inputs for 'from' argument
--FILE--
<?php
/* Test strtr() function: with unexpected inputs for 'from'
 *  and expected type for 'str' & 'to' arguments
*/

echo "*** Testing strtr() function: with unexpected inputs for 'from' ***\n";

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

// array of values for 'from'
$from_arr =  array (

          // integer values
/*1*/	  0,
          1,
          -2,

          // float values
/*4*/	  10.5,
          -20.5,
          10.1234567e10,

          // array values
/*7*/	  array(),
          array(0),
          array(1, 2),

          // boolean values
/*10*/	  true,
          false,
          TRUE,
          FALSE,

          // null values
/*14*/	  NULL,
          null,

          // objects
/*16*/	  new sample(),

          // resource
/*17*/	  $file_handle,
);

//defining 'to' argument
$to = "atm012";

// loop through with each element of the $from array to test strtr() function
$count = 1;
for($index = 0; $index < count($from_arr); $index++) {
  echo "-- Iteration $count --\n";
  $from = $from_arr[$index];
  try {
    var_dump(strtr($str, $from, $to));
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }
  $count++;
}

fclose($file_handle);  //closing the file handle
?>
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for 'from' ***
-- Iteration 1 --
string(6) "a12atm"
-- Iteration 2 --
string(6) "0a2atm"
-- Iteration 3 --
string(6) "01tatm"
-- Iteration 4 --
string(6) "ta2atm"
-- Iteration 5 --
string(6) "m1tatm"
-- Iteration 6 --
string(6) "tm0atm"
-- Iteration 7 --
strtr(): Argument #2 ($from) must be of type string, array given
-- Iteration 8 --
strtr(): Argument #2 ($from) must be of type string, array given
-- Iteration 9 --
strtr(): Argument #2 ($from) must be of type string, array given
-- Iteration 10 --
string(6) "0a2atm"
-- Iteration 11 --
string(6) "012atm"
-- Iteration 12 --
string(6) "0a2atm"
-- Iteration 13 --
string(6) "012atm"
-- Iteration 14 --

Deprecated: strtr(): Passing null to parameter #2 ($from) of type array|string is deprecated in %s on line %d
string(6) "012atm"
-- Iteration 15 --

Deprecated: strtr(): Passing null to parameter #2 ($from) of type array|string is deprecated in %s on line %d
string(6) "012atm"
-- Iteration 16 --
string(6) "012ttm"
-- Iteration 17 --
strtr(): Argument #2 ($from) must be of type string, resource given

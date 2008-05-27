--TEST--
Test strtr() function : usage variations - unexpected inputs for 'str' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'str' 
 *  and expected type for 'from' & 'to' arguments 
*/

echo "*** Testing strtr() function: with unexpected inputs for 'str' ***\n";

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
$strings =  array (

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

//defining 'from' argument
$from = "012atm";

//defining 'to' argument
$to = "atm012";

// loop through with each element of the $strings array to test strtr() function
$count = 1;
for($index = 0; $index < count($strings); $index++) {
  echo "-- Iteration $count --\n";
  $str = $strings[$index];
  var_dump( strtr($str, $from, $to) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for 'str' ***
-- Iteration 1 --
unicode(1) "a"
-- Iteration 2 --
unicode(1) "t"
-- Iteration 3 --
unicode(2) "-m"
-- Iteration 4 --
unicode(4) "ta.5"
-- Iteration 5 --
unicode(5) "-ma.5"
-- Iteration 6 --
unicode(12) "ta5aaaaaaaaa"
-- Iteration 7 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Arr0y"
-- Iteration 8 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Arr0y"
-- Iteration 9 --

Notice: Array to string conversion in %s on line %d
unicode(5) "Arr0y"
-- Iteration 10 --
unicode(1) "t"
-- Iteration 11 --
unicode(0) ""
-- Iteration 12 --
unicode(1) "t"
-- Iteration 13 --
unicode(0) ""
-- Iteration 14 --
unicode(0) ""
-- Iteration 15 --
unicode(0) ""
-- Iteration 16 --
unicode(13) "s02ple objec1"
-- Iteration 17 --
unicode(%d) "Resource id #%d"
-- Iteration 18 --
unicode(0) ""
-- Iteration 19 --
unicode(0) ""
*** Done ***

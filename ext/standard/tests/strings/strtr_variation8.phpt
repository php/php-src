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

  // null values
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
    try {
        var_dump(strtr($str, $replace_pairs));
    } catch (TypeError $e) {
        echo $e->getMessage() . "\n";
    }

    $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECT--
*** Testing strtr() function: with unexpected inputs for 'replace_pairs' ***

-- Iteration 1 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 2 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 3 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 4 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 5 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 6 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 7 --
string(6) "012atm"

-- Iteration 8 --
string(6) "012atm"

-- Iteration 9 --
string(6) "122atm"

-- Iteration 10 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 11 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 12 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 13 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 14 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 15 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 16 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 17 --
strtr(): Argument #2 ($from) must be of type string|array, resource given

-- Iteration 18 --
strtr(): Argument #2 ($from) must be of type array, string given

-- Iteration 19 --
strtr(): Argument #2 ($from) must be of type array, string given
*** Done ***

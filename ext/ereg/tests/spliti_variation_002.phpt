--TEST--
Test spliti() function : usage variations  - unexpected type for arg 2
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing spliti() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$pattern = 'r|j|E';
$limit = 5;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // int data
      0,
      1,
      12345,
      -2345,

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
      .5,

      // array data
      array(),
      array(0),
      array(1),
      array(1, 2),
      array('color' => 'red', 'item' => 'pen'),

      // null data
      NULL,
      null,

      // boolean data
      true,
      false,
      TRUE,
      FALSE,

      // empty data
      "",
      '',

      // object data
      new stdclass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for string

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( spliti($pattern, $value, $limit) );
};

echo "Done";
?>
--EXPECTF--
*** Testing spliti() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(64)
Error: 8 - Undefined variable: unset_var, %s(67)

Arg value 0 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(1) "0"
}

Arg value 1 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(1) "1"
}

Arg value 12345 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(5) "12345"
}

Arg value -2345 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(5) "-2345"
}

Arg value 10.5 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(4) "10.5"
}

Arg value -10.5 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(5) "-10.5"
}

Arg value 101234567000 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(12) "101234567000"
}

Arg value 1.07654321E-9 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(2) {
  [0]=>
  string(10) "1.07654321"
  [1]=>
  string(2) "-9"
}

Arg value 0.5 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(3) "0.5"
}
Error: 8 - Array to string conversion, %sspliti_variation_002.php(%d)

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, array given, %s(74)
NULL
Error: 8 - Array to string conversion, %sspliti_variation_002.php(%d)

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, array given, %s(74)
NULL
Error: 8 - Array to string conversion, %sspliti_variation_002.php(%d)

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, array given, %s(74)
NULL
Error: 8 - Array to string conversion, %sspliti_variation_002.php(%d)

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, array given, %s(74)
NULL
Error: 8 - Array to string conversion, %sspliti_variation_002.php(%d)

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, array given, %s(74)
NULL

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value 1 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(1) "1"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value 1 
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(1) "1"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}
Error: 4096 - Object of class stdClass could not be converted to string, %s(73)

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
Error: 2 - spliti() expects parameter 2 to be string, object given, %s(74)
NULL

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(74)
array(1) {
  [0]=>
  string(0) ""
}
Done

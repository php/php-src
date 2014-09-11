--TEST--
Test eregi() function : usage variations  - unexpected type for arg 3
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing eregi() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$pattern = 'h(.*)lo!';
$string = 'hello!';

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

      // string data
      "string",
      'string',

      // object data
      new stdclass(),

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for registers

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( eregi($pattern, $string, $value) );
      var_dump($value);
};

echo "Done";
?>
--EXPECTF--
*** Testing eregi() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(61)
Error: 8 - Undefined variable: unset_var, %s(64)

Arg value 0 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 1 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 12345 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value -2345 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 10.5 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value -10.5 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 101234567000 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 1.07654321E-9 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 0.5 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 1 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value 1 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value string 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value string 
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}
Error: 4096 - Object of class stdClass could not be converted to string, %s(70)

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}

Arg value  
Error: 8192 - Function eregi() is deprecated, %s(71)
int(6)
array(2) {
  [0]=>
  string(6) "hello!"
  [1]=>
  string(2) "el"
}
Done

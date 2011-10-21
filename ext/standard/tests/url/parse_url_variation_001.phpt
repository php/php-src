--TEST--
Test parse_url() function : usage variations  - unexpected type for arg 1.
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components 
 * Source code: ext/standard/url.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing parse_url() : usage variations ***\n";

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

// loop through each element of the array for url

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( parse_url($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing parse_url() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(60)
Error: 8 - Undefined variable: unset_var, %s(63)

Arg value 0 
array(1) {
  ["path"]=>
  string(1) "0"
}

Arg value 1 
array(1) {
  ["path"]=>
  string(1) "1"
}

Arg value 12345 
array(1) {
  ["path"]=>
  string(5) "12345"
}

Arg value -2345 
array(1) {
  ["path"]=>
  string(5) "-2345"
}

Arg value 10.5 
array(1) {
  ["path"]=>
  string(4) "10.5"
}

Arg value -10.5 
array(1) {
  ["path"]=>
  string(5) "-10.5"
}

Arg value 101234567000 
array(1) {
  ["path"]=>
  string(12) "101234567000"
}

Arg value 1.07654321E-9 
array(1) {
  ["path"]=>
  string(13) "1.07654321E-9"
}

Arg value 0.5 
array(1) {
  ["path"]=>
  string(3) "0.5"
}
Error: 8 - Array to string conversion, %sparse_url_variation_001.php(%d)

Arg value Array 
Error: 2 - parse_url() expects parameter 1 to be string, array given, %s(70)
NULL
Error: 8 - Array to string conversion, %sparse_url_variation_001.php(%d)

Arg value Array 
Error: 2 - parse_url() expects parameter 1 to be string, array given, %s(70)
NULL
Error: 8 - Array to string conversion, %sparse_url_variation_001.php(%d)

Arg value Array 
Error: 2 - parse_url() expects parameter 1 to be string, array given, %s(70)
NULL
Error: 8 - Array to string conversion, %sparse_url_variation_001.php(%d)

Arg value Array 
Error: 2 - parse_url() expects parameter 1 to be string, array given, %s(70)
NULL
Error: 8 - Array to string conversion, %sparse_url_variation_001.php(%d)

Arg value Array 
Error: 2 - parse_url() expects parameter 1 to be string, array given, %s(70)
NULL

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value 1 
array(1) {
  ["path"]=>
  string(1) "1"
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value 1 
array(1) {
  ["path"]=>
  string(1) "1"
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}
Error: 4096 - Object of class stdClass could not be converted to string, %s(69)

Arg value  
Error: 2 - parse_url() expects parameter 1 to be string, object given, %s(70)
NULL

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}

Arg value  
array(1) {
  ["path"]=>
  string(0) ""
}
Done
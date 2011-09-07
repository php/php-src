--TEST--
Test curl_version() function : usage variations - test values for $ascii argument
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	echo "skip - curl extension not available in this build";
}
if (!getenv('PHP_CURL_HTTP_REMOTE_SERVER')) {
	echo "skip need PHP_CURL_HTTP_REMOTE_SERVER environment variable";
}
?>
--FILE--
<?php

/* Prototype  : array curl_version  ([ int $age  ] )
 * Description: Returns information about the cURL version.
 * Source code: ext/curl/interface.c
*/

echo "*** Testing curl_version() function: with unexpected inputs for 'age' argument ***\n";

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

// array with different values for $input
$inputs =  array (

  // integer values
  0,
  1,
  255,
  256,
  PHP_INT_MAX,
  -PHP_INT_MAX,

  // float values
  10.5,
  -20.5,
  10.1234567e10,

  // array values
  array(),
  array(0),
  array(1, 2),
  
  //string values
  "ABC",
  'abc',
  "2abc",

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

// loop through with each element of the $inputs array to test curl_version() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( is_array(curl_version($input)) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===Done===
--EXPECTF--
*** Testing curl_version() function: with unexpected inputs for 'age' argument ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(true)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(true)
-- Iteration 10 --

Warning: curl_version() expects parameter 1 to be long, array given in %s on line %d
bool(false)
-- Iteration 11 --

Warning: curl_version() expects parameter 1 to be long, array given in %s on line %d
bool(false)
-- Iteration 12 --

Warning: curl_version() expects parameter 1 to be long, array given in %s on line %d
bool(false)
-- Iteration 13 --

Warning: curl_version() expects parameter 1 to be long, string given in %s on line %d
bool(false)
-- Iteration 14 --

Warning: curl_version() expects parameter 1 to be long, string given in %s on line %d
bool(false)
-- Iteration 15 --

Notice: A non well formed numeric value encountered in %s on line %d
bool(true)
-- Iteration 16 --
bool(true)
-- Iteration 17 --
bool(true)
-- Iteration 18 --
bool(true)
-- Iteration 19 --
bool(true)
-- Iteration 20 --
bool(true)
-- Iteration 21 --
bool(true)
-- Iteration 22 --

Warning: curl_version() expects parameter 1 to be long, object given in %s on line %d
bool(false)
-- Iteration 23 --

Warning: curl_version() expects parameter 1 to be long, resource given in %s on line %d
bool(false)
-- Iteration 24 --
bool(true)
-- Iteration 25 --
bool(true)
===Done===

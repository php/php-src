--TEST--
Test parse_url() function : usage variations  - unexpected type for arg 2.
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only"); ?>
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

// Initialise function arguments not being substituted (if any)
$url = 'http://secret:hideout@www.php.net:80/index.php?test=1&test2=char&test3=mixesCI#some_page_ref123';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

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

// loop through each element of the array for url_component

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( parse_url($url, $value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing parse_url() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(61)
Error: 8 - Undefined variable: unset_var, %s(64)

Arg value 10.5 
Error: 2 - parse_url(): Invalid URL component identifier 10, %s(71)
bool(false)

Arg value -10.5 
array(8) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(11) "www.php.net"
  ["port"]=>
  int(80)
  ["user"]=>
  string(6) "secret"
  ["pass"]=>
  string(7) "hideout"
  ["path"]=>
  string(10) "/index.php"
  ["query"]=>
  string(31) "test=1&test2=char&test3=mixesCI"
  ["fragment"]=>
  string(16) "some_page_ref123"
}

Arg value 101234567000 
Error: 2 - parse_url(): Invalid URL component identifier %d, %s(71)
bool(false)

Arg value 1.07654321E-9 
string(4) "http"

Arg value 0.5 
string(4) "http"

Arg value Array 
Error: 2 - parse_url() expects parameter 2 to be long, array given, %s(71)
NULL

Arg value Array 
Error: 2 - parse_url() expects parameter 2 to be long, array given, %s(71)
NULL

Arg value Array 
Error: 2 - parse_url() expects parameter 2 to be long, array given, %s(71)
NULL

Arg value Array 
Error: 2 - parse_url() expects parameter 2 to be long, array given, %s(71)
NULL

Arg value Array 
Error: 2 - parse_url() expects parameter 2 to be long, array given, %s(71)
NULL

Arg value  
string(4) "http"

Arg value  
string(4) "http"

Arg value 1 
string(11) "www.php.net"

Arg value  
string(4) "http"

Arg value 1 
string(11) "www.php.net"

Arg value  
string(4) "http"

Arg value  
Error: 2 - parse_url() expects parameter 2 to be long, string given, %s(71)
NULL

Arg value  
Error: 2 - parse_url() expects parameter 2 to be long, string given, %s(71)
NULL

Arg value string 
Error: 2 - parse_url() expects parameter 2 to be long, string given, %s(71)
NULL

Arg value string 
Error: 2 - parse_url() expects parameter 2 to be long, string given, %s(71)
NULL
Error: 4096 - Object of class stdClass could not be converted to string, %s(70)

Arg value  
Error: 2 - parse_url() expects parameter 2 to be long, object given, %s(71)
NULL

Arg value  
string(4) "http"

Arg value  
string(4) "http"
Done
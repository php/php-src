--TEST--
Test spliti() function : usage variations  - unexpected type for arg 3
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');
echo "*** Testing spliti() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$pattern = b'[[:space:]]';
$string = '1 2 3 4 5';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // float data
      10.5,
      -10.5,
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
      @$undefined_var,

      // unset data
      @$unset_var,
);

// loop through each element of the array for limit

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( spliti($pattern, $string, $value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing spliti() : usage variations ***

Arg value 10.5 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "4"
  [4]=>
  string(1) "5"
}

Arg value -10.5 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value 1.07654321E-9 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value 0.5 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, array given, %s(73)
NULL

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, array given, %s(73)
NULL

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, array given, %s(73)
NULL

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, array given, %s(73)
NULL

Arg value Array 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, array given, %s(73)
NULL

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value 1 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value 1 
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, string given, %s(73)
NULL

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, string given, %s(73)
NULL

Arg value string 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, string given, %s(73)
NULL

Arg value string 
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, string given, %s(73)
NULL
Error: 4096 - Object of class stdClass could not be converted to string, %s(72)

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
Error: 2 - spliti() expects parameter 3 to be long, object given, %s(73)
NULL

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}

Arg value  
Error: 8192 - Function spliti() is deprecated, %s(73)
array(1) {
  [0]=>
  string(9) "1 2 3 4 5"
}
Done
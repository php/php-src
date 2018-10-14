--TEST--
Test strftime() function : usage variation - Passing unexpected values to first argument 'format'.
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");

// Initialise function arguments not being substituted (if any)
$timestamp = 10;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define some classes
class classWithToString
{
	public function __toString() {
		return "Class A object";
	}
}

class classWithoutToString
{
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

//array of values to iterate over
$inputs = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -12345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
      'float .5' => .5,

      // array data
      'empty array' => array(),
      'int indexed array' => $index_array,
      'associative array' => $assoc_array,
      'nested arrays' => array('foo', $index_array, $assoc_array),

      // null data
      'uppercase NULL' => NULL,
      'lowercase null' => null,

      // boolean data
      'lowercase true' => true,
      'lowercase false' =>false,
      'uppercase TRUE' =>TRUE,
      'uppercase FALSE' =>FALSE,

      // empty data
      'empty string DQ' => "",
      'empty string SQ' => '',

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
);

// loop through each element of the array for format

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( strftime($value) );
      var_dump( strftime($value, $timestamp) );
};

?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

--int 0--
string(1) "0"
string(1) "0"

--int 1--
string(1) "1"
string(1) "1"

--int 12345--
string(5) "12345"
string(5) "12345"

--int -12345--
string(6) "-12345"
string(6) "-12345"

--float 10.5--
string(4) "10.5"
string(4) "10.5"

--float -10.5--
string(5) "-10.5"
string(5) "-10.5"

--float 12.3456789000e10--
string(12) "123456789000"
string(12) "123456789000"

--float -12.3456789000e10--
string(13) "-123456789000"
string(13) "-123456789000"

--float .5--
string(3) "0.5"
string(3) "0.5"

--empty array--

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--int indexed array--

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--associative array--

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--nested arrays--

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: strftime() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--uppercase NULL--
bool(false)
bool(false)

--lowercase null--
bool(false)
bool(false)

--lowercase true--
string(1) "1"
string(1) "1"

--lowercase false--
bool(false)
bool(false)

--uppercase TRUE--
string(1) "1"
string(1) "1"

--uppercase FALSE--
bool(false)
bool(false)

--empty string DQ--
bool(false)
bool(false)

--empty string SQ--
bool(false)
bool(false)

--instance of classWithToString--
string(14) "Class A object"
string(14) "Class A object"

--instance of classWithoutToString--

Warning: strftime() expects parameter 1 to be string, object given in %s on line %d
bool(false)

Warning: strftime() expects parameter 1 to be string, object given in %s on line %d
bool(false)

--undefined var--
bool(false)
bool(false)

--unset var--
bool(false)
bool(false)
===DONE===

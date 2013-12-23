--TEST--
Test strftime() function : usage variation - Passing unexpected values to second argument 'timestamp'.
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing strftime() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");
// Initialise all required variables
$format = '%b %d %Y %H:%M:%S';

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

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
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

      // string data
      'string DQ' => "string",
      'string SQ' => 'string',
      'mixed case string' => "sTrInG",
      'heredoc' => $heredoc,

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( strftime($format, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

--float 10.5--
string(20) "Jan 01 1970 05:30:10"

--float -10.5--
string(20) "Jan 01 1970 05:29:50"

--float .5--
string(20) "Jan 01 1970 05:30:00"

--empty array--

Warning: strftime() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--int indexed array--

Warning: strftime() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--associative array--

Warning: strftime() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--nested arrays--

Warning: strftime() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--uppercase NULL--
string(20) "Jan 01 1970 05:30:00"

--lowercase null--
string(20) "Jan 01 1970 05:30:00"

--lowercase true--
string(20) "Jan 01 1970 05:30:01"

--lowercase false--
string(20) "Jan 01 1970 05:30:00"

--uppercase TRUE--
string(20) "Jan 01 1970 05:30:01"

--uppercase FALSE--
string(20) "Jan 01 1970 05:30:00"

--empty string DQ--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string DQ--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string SQ--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--mixed case string--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--heredoc--

Warning: strftime() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: strftime() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: strftime() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--undefined var--
string(20) "Jan 01 1970 05:30:00"

--unset var--
string(20) "Jan 01 1970 05:30:00"
===DONE===

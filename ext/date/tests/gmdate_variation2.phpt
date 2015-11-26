--TEST--
Test gmdate() function : usage variation - Passing unexpected values to timestamp argument.
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing gmdate() : usage variation ***\n";


// Initialise all required variables
date_default_timezone_set('UTC');
$format = DATE_ISO8601;

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
      var_dump( gmdate($format, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

--int 0--
string(24) "1970-01-01T00:00:00+0000"

--int 1--
string(24) "1970-01-01T00:00:01+0000"

--int 12345--
string(24) "1970-01-01T03:25:45+0000"

--int -12345--
string(24) "1969-12-31T20:34:15+0000"

--float 10.5--
string(24) "1970-01-01T00:00:10+0000"

--float -10.5--
string(24) "1969-12-31T23:59:50+0000"

--float .5--
string(24) "1970-01-01T00:00:00+0000"

--empty array--

Warning: gmdate() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--int indexed array--

Warning: gmdate() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--associative array--

Warning: gmdate() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--nested arrays--

Warning: gmdate() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--uppercase NULL--
string(24) "1970-01-01T00:00:00+0000"

--lowercase null--
string(24) "1970-01-01T00:00:00+0000"

--lowercase true--
string(24) "1970-01-01T00:00:01+0000"

--lowercase false--
string(24) "1970-01-01T00:00:00+0000"

--uppercase TRUE--
string(24) "1970-01-01T00:00:01+0000"

--uppercase FALSE--
string(24) "1970-01-01T00:00:00+0000"

--empty string DQ--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string DQ--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string SQ--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--mixed case string--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--heredoc--

Warning: gmdate() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: gmdate() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: gmdate() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--undefined var--
string(24) "1970-01-01T00:00:00+0000"

--unset var--
string(24) "1970-01-01T00:00:00+0000"
===DONE===

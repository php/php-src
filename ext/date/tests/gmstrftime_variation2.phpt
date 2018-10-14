--TEST--
Test gmstrftime() function : usage variation - Passing unexpected values to second argument 'timestamp'.
--SKIPIF--
<?php
if(PHP_INT_SIZE != 4 ) {
  die("skip Test is not valid for 64-bit");
}
?>
--FILE--
<?php
/* Prototype  : string gmstrftime(string format [, int timestamp])
 * Description: Format a GMT/UCT time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing gmstrftime() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");

// Initialise function arguments not being substituted (if any)
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
      var_dump( gmstrftime($format, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmstrftime() : usage variation ***

--float 10.5--
string(20) "Jan 01 1970 00:00:10"

--float -10.5--
string(20) "Dec 31 1969 23:59:50"

--float 12.3456789000e10--

Warning: gmstrftime() expects parameter 2 to be int, float given in %s on line %d
bool(false)

--float -12.3456789000e10--

Warning: gmstrftime() expects parameter 2 to be int, float given in %s on line %d
bool(false)

--float .5--
string(20) "Jan 01 1970 00:00:00"

--empty array--

Warning: gmstrftime() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--int indexed array--

Warning: gmstrftime() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--associative array--

Warning: gmstrftime() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--nested arrays--

Warning: gmstrftime() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--uppercase NULL--
string(20) "Jan 01 1970 00:00:00"

--lowercase null--
string(20) "Jan 01 1970 00:00:00"

--lowercase true--
string(20) "Jan 01 1970 00:00:01"

--lowercase false--
string(20) "Jan 01 1970 00:00:00"

--uppercase TRUE--
string(20) "Jan 01 1970 00:00:01"

--uppercase FALSE--
string(20) "Jan 01 1970 00:00:00"

--empty string DQ--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--string DQ--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--string SQ--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--mixed case string--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--heredoc--

Warning: gmstrftime() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: gmstrftime() expects parameter 2 to be int, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: gmstrftime() expects parameter 2 to be int, object given in %s on line %d
bool(false)

--undefined var--
string(20) "Jan 01 1970 00:00:00"

--unset var--
string(20) "Jan 01 1970 00:00:00"
===DONE===

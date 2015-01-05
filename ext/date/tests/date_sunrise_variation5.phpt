--TEST--
Test date_sunrise() function : usage variation - Passing unexpected values to fifth argument zenith
--FILE--
<?php
/* Prototype  : mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunrise for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunrise() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
date_default_timezone_set("Asia/Calcutta");
$time = mktime(8, 8, 8, 8, 8, 2008);
$latitude = 38.4;
$longitude = -9;
$gmt_offset = 5.5;

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

// loop through each element of the array for zenith

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( date_sunrise($time, SUNFUNCS_RET_STRING, $latitude, $longitude, $value, $gmt_offset) );
      var_dump( date_sunrise($time, SUNFUNCS_RET_DOUBLE, $latitude, $longitude, $value, $gmt_offset) );
      var_dump( date_sunrise($time, SUNFUNCS_RET_TIMESTAMP, $latitude, $longitude, $value, $gmt_offset) );
};

?>
===DONE===
--EXPECTF--
*** Testing date_sunrise() : usage variation ***

--int 0--
bool(false)
bool(false)
bool(false)

--int 1--
bool(false)
bool(false)
bool(false)

--int 12345--
string(5) "09:51"
float(9.855%d)
int(1218169278)

--int -12345--
string(5) "09:54"
float(9.9097820911118)
int(1218169475)

--empty array--

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

--int indexed array--

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

--associative array--

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

--nested arrays--

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, array given in %s on line %d
bool(false)

--uppercase NULL--
bool(false)
bool(false)
bool(false)

--lowercase null--
bool(false)
bool(false)
bool(false)

--lowercase true--
bool(false)
bool(false)
bool(false)

--lowercase false--
bool(false)
bool(false)
bool(false)

--uppercase TRUE--
bool(false)
bool(false)
bool(false)

--uppercase FALSE--
bool(false)
bool(false)
bool(false)

--empty string DQ--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--string DQ--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--string SQ--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--mixed case string--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--heredoc--

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

Warning: date_sunrise() expects parameter 5 to be float, object given in %s on line %d
bool(false)

--undefined var--
bool(false)
bool(false)
bool(false)

--unset var--
bool(false)
bool(false)
bool(false)
===DONE===

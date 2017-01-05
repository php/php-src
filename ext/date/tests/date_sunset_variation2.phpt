--TEST--
Test date_sunset() function : usage variation - Passing unexpected values to second argument format.
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
/* Prototype  : mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
 * Description: Returns time of sunset for a given day and location 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing date_sunset() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
date_default_timezone_set("Asia/Calcutta");
$time = mktime(8, 8, 8, 8, 8, 2008);
$latitude = 22.34;
$longitude = 88.21;
$zenith = 90;
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

// loop through each element of the array for format

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( date_sunset($time, $value, $latitude, $longitude, $zenith, $gmt_offset) );
};

?>
===DONE===
--EXPECTF--
*** Testing date_sunset() : usage variation ***

--float 10.5--

Warning: date_sunset(): Wrong return format given, pick one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING or SUNFUNCS_RET_DOUBLE in %s on line %d
bool(false)

--float -10.5--

Warning: date_sunset(): Wrong return format given, pick one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING or SUNFUNCS_RET_DOUBLE in %s on line %d
bool(false)

--float 12.3456789000e10--

Warning: date_sunset() expects parameter 2 to be integer, float given in %s on line %d
bool(false)

--float -12.3456789000e10--

Warning: date_sunset() expects parameter 2 to be integer, float given in %s on line %d
bool(false)

--float .5--
int(1218199253)

--empty array--

Warning: date_sunset() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--int indexed array--

Warning: date_sunset() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--associative array--

Warning: date_sunset() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--nested arrays--

Warning: date_sunset() expects parameter 2 to be integer, array given in %s on line %d
bool(false)

--uppercase NULL--
int(1218199253)

--lowercase null--
int(1218199253)

--lowercase true--
string(5) "18:10"

--lowercase false--
int(1218199253)

--uppercase TRUE--
string(5) "18:10"

--uppercase FALSE--
int(1218199253)

--empty string DQ--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string DQ--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--string SQ--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--mixed case string--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--heredoc--

Warning: date_sunset() expects parameter 2 to be integer, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: date_sunset() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: date_sunset() expects parameter 2 to be integer, object given in %s on line %d
bool(false)

--undefined var--
int(1218199253)

--unset var--
int(1218199253)
===DONE===
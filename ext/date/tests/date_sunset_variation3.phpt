--TEST--
Test date_sunset() function : usage variation - Passing unexpected values to third argument latitude.
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

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -2345,

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

// loop through each element of the array for latitude

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( date_sunset($time, SUNFUNCS_RET_STRING, $value, $longitude, $zenith, $gmt_offset) );
      var_dump( date_sunset($time, SUNFUNCS_RET_DOUBLE, $value, $longitude, $zenith, $gmt_offset) );
      var_dump( date_sunset($time, SUNFUNCS_RET_TIMESTAMP, $value, $longitude, $zenith, $gmt_offset) );
      
};

?>
===DONE===
--EXPECTF--
*** Testing date_sunset() : usage variation ***

--int 0--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--int 1--
string(5) "17:44"
float(17.749%d)
int(1218197698)

--int 12345--
bool(false)
bool(false)
bool(false)

--int -12345--
string(5) "17:35"
float(17.598%d)
int(1218197155)

--empty array--

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

--int indexed array--

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

--associative array--

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

--nested arrays--

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, array given in %s on line %d
bool(false)

--uppercase NULL--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--lowercase null--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--lowercase true--
string(5) "17:44"
float(17.749%d)
int(1218197698)

--lowercase false--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--uppercase TRUE--
string(5) "17:44"
float(17.749%d)
int(1218197698)

--uppercase FALSE--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--empty string DQ--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--string DQ--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--string SQ--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--mixed case string--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--heredoc--

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

Warning: date_sunset() expects parameter 3 to be double, object given in %s on line %d
bool(false)

--undefined var--
string(5) "17:43"
float(17.730%d)
int(1218197630)

--unset var--
string(5) "17:43"
float(17.730%d)
int(1218197630)
===DONE===

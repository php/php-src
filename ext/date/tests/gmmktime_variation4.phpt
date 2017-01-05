--TEST--
Test gmmktime() function : usage variation - Passing unexpected values to fourth argument month.
--FILE--
<?php
/* Prototype  : int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
 * Description: Get UNIX timestamp for a GMT date 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing gmmktime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$hour = 8;
$min = 8;
$sec = 8;
$day = 8;
$year = 2008;

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

// loop through each element of the array for mon

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gmmktime($hour, $min, $sec, $value, $day, $year) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmmktime() : usage variation ***

--float 10.5--
int(1223453288)

--float -10.5--
int(1170922088)

--float .5--
int(1197101288)

--empty array--

Warning: gmmktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

--int indexed array--

Warning: gmmktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

--associative array--

Warning: gmmktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

--nested arrays--

Warning: gmmktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

--uppercase NULL--
int(1197101288)

--lowercase null--
int(1197101288)

--lowercase true--
int(1199779688)

--lowercase false--
int(1197101288)

--uppercase TRUE--
int(1199779688)

--uppercase FALSE--
int(1197101288)

--empty string DQ--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--string DQ--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--string SQ--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--mixed case string--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--heredoc--

Warning: gmmktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: gmmktime() expects parameter 4 to be integer, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: gmmktime() expects parameter 4 to be integer, object given in %s on line %d
bool(false)

--undefined var--
int(1197101288)

--unset var--
int(1197101288)
===DONE===

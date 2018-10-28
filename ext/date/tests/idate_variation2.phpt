--TEST--
Test idate() function : usage variation - Passing unexpected values to second optional argument 'timestamp'.
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$format = 'Y';
date_default_timezone_set("Asia/Calcutta");

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
      var_dump( idate($format, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing idate() : usage variation ***

--float 10.5--
int(1970)

--float -10.5--
int(1970)

--float .5--
int(1970)

--empty array--

Warning: idate() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--int indexed array--

Warning: idate() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--associative array--

Warning: idate() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--nested arrays--

Warning: idate() expects parameter 2 to be int, array given in %s on line %d
bool(false)

--uppercase NULL--
int(1970)

--lowercase null--
int(1970)

--lowercase true--
int(1970)

--lowercase false--
int(1970)

--uppercase TRUE--
int(1970)

--uppercase FALSE--
int(1970)

--empty string DQ--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--empty string SQ--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--string DQ--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--string SQ--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--mixed case string--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--heredoc--

Warning: idate() expects parameter 2 to be int, string given in %s on line %d
bool(false)

--instance of classWithToString--

Warning: idate() expects parameter 2 to be int, object given in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: idate() expects parameter 2 to be int, object given in %s on line %d
bool(false)

--undefined var--
int(1970)

--unset var--
int(1970)
===DONE===

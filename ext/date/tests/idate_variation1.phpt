--TEST--
Test idate() function : usage variation - Passing unexpected values to first argument 'format'.
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

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
      var_dump( idate($value) );
      var_dump( idate($value, $timestamp) );
};

?>
===DONE===
--EXPECTF--
*** Testing idate() : usage variation ***

--int 0--

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

--int 1--

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

--int 12345--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--int -12345--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--float 10.5--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--float -10.5--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--float 12.3456789000e10--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--float -12.3456789000e10--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--float .5--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--empty array--

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--int indexed array--

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--associative array--

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--nested arrays--

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: idate() expects parameter 1 to be string, array given in %s on line %d
bool(false)

--uppercase NULL--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--lowercase null--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--lowercase true--

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

--lowercase false--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--uppercase TRUE--

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

Warning: idate(): Unrecognized date format token. in %s on line %d
bool(false)

--uppercase FALSE--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--empty string DQ--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--empty string SQ--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--instance of classWithToString--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: idate() expects parameter 1 to be string, object given in %s on line %d
bool(false)

Warning: idate() expects parameter 1 to be string, object given in %s on line %d
bool(false)

--undefined var--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)

--unset var--

Warning: idate(): idate format is one char in %s on line %d
bool(false)

Warning: idate(): idate format is one char in %s on line %d
bool(false)
===DONE===

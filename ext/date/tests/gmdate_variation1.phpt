--TEST--
Test gmdate() function : usage variation - Passing unexpected values to format argument .
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
      var_dump( gmdate($value, $timestamp) );
      var_dump( gmdate($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

--int 0--
unicode(1) "0"
unicode(1) "0"

--int 1--
unicode(1) "1"
unicode(1) "1"

--int 12345--
unicode(5) "12345"
unicode(5) "12345"

--int -12345--
unicode(6) "-12345"
unicode(6) "-12345"

--float 10.5--
unicode(4) "10.5"
unicode(4) "10.5"

--float -10.5--
unicode(5) "-10.5"
unicode(5) "-10.5"

--float 12.3456789000e10--
unicode(12) "123456789000"
unicode(12) "123456789000"

--float -12.3456789000e10--
unicode(13) "-123456789000"
unicode(13) "-123456789000"

--float .5--
unicode(3) "0.5"
unicode(3) "0.5"

--empty array--

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

--int indexed array--

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

--associative array--

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

--nested arrays--

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

Warning: gmdate() expects parameter 1 to be binary string, array given in %s on line %d
bool(false)

--uppercase NULL--
unicode(0) ""
unicode(0) ""

--lowercase null--
unicode(0) ""
unicode(0) ""

--lowercase true--
unicode(1) "1"
unicode(1) "1"

--lowercase false--
unicode(0) ""
unicode(0) ""

--uppercase TRUE--
unicode(1) "1"
unicode(1) "1"

--uppercase FALSE--
unicode(0) ""
unicode(0) ""

--empty string DQ--
unicode(0) ""
unicode(0) ""

--empty string SQ--
unicode(0) ""
unicode(0) ""

--instance of classWithToString--
unicode(53) "CFridayam0808 AM 2008b8UTC2008-08-08T08:08:08+00:0031"
unicode(%d) "%s"

--instance of classWithoutToString--

Warning: gmdate() expects parameter 1 to be binary string, object given in %s on line %d
bool(false)

Warning: gmdate() expects parameter 1 to be binary string, object given in %s on line %d
bool(false)

--undefined var--
unicode(0) ""
unicode(0) ""

--unset var--
unicode(0) ""
unicode(0) ""
===DONE===

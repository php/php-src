--TEST--
Test gettimeofday() function : usage variation - Passing unexpected values to get_as_float argument
--FILE--
<?php
/* Prototype  : array gettimeofday([bool get_as_float])
 * Description: Returns the current time as array
 * Source code: ext/standard/microtime.c
 * Alias to functions:
 */

echo "*** Testing gettimeofday() : usage variation ***\n";

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

// loop through each element of the array for get_as_float

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( gettimeofday($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing gettimeofday() : usage variation ***

--int 0--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--int 1--
float(%f)

--int 12345--
float(%f)

--int -12345--
float(%f)

--float 10.5--
float(%f)

--float -10.5--
float(%f)

--float 12.3456789000e10--
float(%f)

--float -12.3456789000e10--
float(%f)

--float .5--
float(%f)

--empty array--

Warning: gettimeofday() expects parameter 1 to be boolean, array given in %s on line %d
NULL

--int indexed array--

Warning: gettimeofday() expects parameter 1 to be boolean, array given in %s on line %d
NULL

--associative array--

Warning: gettimeofday() expects parameter 1 to be boolean, array given in %s on line %d
NULL

--nested arrays--

Warning: gettimeofday() expects parameter 1 to be boolean, array given in %s on line %d
NULL

--uppercase NULL--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--lowercase null--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--lowercase true--
float(%f)

--lowercase false--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--uppercase TRUE--
float(%f)

--uppercase FALSE--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--empty string DQ--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--empty string SQ--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--string DQ--
float(%f)

--string SQ--
float(%f)

--mixed case string--
float(%f)

--heredoc--
float(%f)

--instance of classWithToString--

Warning: gettimeofday() expects parameter 1 to be boolean, object given in %s on line %d
NULL

--instance of classWithoutToString--

Warning: gettimeofday() expects parameter 1 to be boolean, object given in %s on line %d
NULL

--undefined var--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}

--unset var--
array(4) {
  ["sec"]=>
  int(%d)
  ["usec"]=>
  int(%d)
  ["minuteswest"]=>
  int(-330)
  ["dsttime"]=>
  int(0)
}
===DONE===

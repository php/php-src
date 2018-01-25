--TEST--
Test date_parse() function : usage variation - Passing unexpected values to first argument $date.
--FILE--
<?php
/* Prototype  : array date_parse  ( string $date  )
 * Description: Returns associative array with detailed info about given date.
 * Source code: ext/date/php_date.c
 */

echo "*** Testing date_parse() : usage variation -  unexpected values to first argument \$date***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

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

// resource
$file_handle = fopen(__FILE__, 'r');

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

      // resource
      'resource' => $file_handle
);

foreach($inputs as $variation =>$date) {
      echo "\n-- $variation --\n";
      $result = date_parse($date);
      if (is_array($result)) {
      	  var_dump($result["errors"]);
      } else {
      	  var_dump($result);
      }
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing date_parse() : usage variation -  unexpected values to first argument $date***

-- int 0 --
array(1) {
  [0]=>
  string(20) "Unexpected character"
}

-- int 1 --
array(1) {
  [0]=>
  string(20) "Unexpected character"
}

-- int 12345 --
array(1) {
  [4]=>
  string(20) "Unexpected character"
}

-- int -12345 --
array(1) {
  [5]=>
  string(20) "Unexpected character"
}

-- float 10.5 --
array(0) {
}

-- float -10.5 --
array(1) {
  [4]=>
  string(20) "Unexpected character"
}

-- float .5 --
array(0) {
}

-- empty array --

Warning: date_parse() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: date_parse() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- associative array --

Warning: date_parse() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: date_parse() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- uppercase NULL --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- lowercase null --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- lowercase true --
array(1) {
  [0]=>
  string(20) "Unexpected character"
}

-- lowercase false --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- uppercase TRUE --
array(1) {
  [0]=>
  string(20) "Unexpected character"
}

-- uppercase FALSE --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- empty string DQ --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- empty string SQ --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- string DQ --
array(1) {
  [0]=>
  string(47) "The timezone could not be found in the database"
}

-- string SQ --
array(1) {
  [0]=>
  string(47) "The timezone could not be found in the database"
}

-- mixed case string --
array(1) {
  [0]=>
  string(47) "The timezone could not be found in the database"
}

-- heredoc --
array(1) {
  [0]=>
  string(47) "The timezone could not be found in the database"
}

-- instance of classWithToString --
array(2) {
  [0]=>
  string(47) "The timezone could not be found in the database"
  [8]=>
  string(29) "Double timezone specification"
}

-- instance of classWithoutToString --

Warning: date_parse() expects parameter 1 to be string, object given in %s on line %d
bool(false)

-- undefined var --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- unset var --
array(1) {
  [0]=>
  string(12) "Empty string"
}

-- resource --

Warning: date_parse() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===DONE===

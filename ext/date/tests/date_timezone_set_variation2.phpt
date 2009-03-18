--TEST--
Test date_timezone_set() function : usage variation - Passing unexpected values to second argument $timezone.
--FILE--
<?php
/* Prototype  : DateTime date_timezone_set  ( DateTime $object  , DateTimeZone $timezone  )
 * Description: Sets the time zone for the DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::setTimezone
 */

echo "*** Testing date_timezone_set() : usage variation -  unexpected values to second argument \$timezone***\n";

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

$object = date_create("2009-01-30 17:57:32");

foreach($inputs as $variation =>$timezone) {
      echo "\n-- $variation --\n";
      var_dump( date_timezone_set($object, $timezone) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing date_timezone_set() : usage variation -  unexpected values to second argument $timezone***

-- int 0 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int 1 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int 12345 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int -12345 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- float 10.5 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, double given in %s on line %d
bool(false)

-- float -10.5 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, double given in %s on line %d
bool(false)

-- float .5 --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, double given in %s on line %d
bool(false)

-- empty array --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, array given in %s on line %d
bool(false)

-- associative array --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, array given in %s on line %d
bool(false)

-- uppercase NULL --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, null given in %s on line %d
bool(false)

-- lowercase null --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, null given in %s on line %d
bool(false)

-- lowercase true --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- lowercase false --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- uppercase TRUE --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- uppercase FALSE --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- empty string DQ --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- string DQ --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- string SQ --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- heredoc --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, object given in %s on line %d
bool(false)

-- undefined var --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, null given in %s on line %d
bool(false)

-- unset var --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, null given in %s on line %d
bool(false)

-- resource --

Warning: date_timezone_set() expects parameter 2 to be DateTimeZone, resource given in %s on line %d
bool(false)
===DONE===

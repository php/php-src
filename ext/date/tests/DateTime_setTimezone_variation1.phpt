--TEST--
Test DateTime::setTimezone() function : usage variation - Passing unexpected values to first argument $timezone.
--FILE--
<?php
/* Prototype  : public DateTime DateTime::setTimezone  ( DateTimeZone $timezone  )
 * Description: Sets the time zone for the DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: date_timezone_set
 */

echo "*** Testing DateTime::setTimezone() : usage variation -  unexpected values to first argument \$timezone***\n";

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

$object = new DateTime("2009-01-30 17:57:32");

foreach($inputs as $variation =>$timezone) {
      echo "\n-- $variation --\n";
      var_dump( $object->setTimezone($timezone) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setTimezone() : usage variation -  unexpected values to first argument $timezone***

-- int 0 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int 1 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int 12345 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- int -12345 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, integer given in %s on line %d
bool(false)

-- float 10.5 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, double given in %s on line %d
bool(false)

-- float -10.5 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, double given in %s on line %d
bool(false)

-- float .5 --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, double given in %s on line %d
bool(false)

-- empty array --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, array given in %s on line %d
bool(false)

-- associative array --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, array given in %s on line %d
bool(false)

-- uppercase NULL --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, null given in %s on line %d
bool(false)

-- lowercase null --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, null given in %s on line %d
bool(false)

-- lowercase true --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- lowercase false --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- uppercase TRUE --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- uppercase FALSE --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, boolean given in %s on line %d
bool(false)

-- empty string DQ --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- string DQ --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- string SQ --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- heredoc --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, object given in %s on line %d
bool(false)

-- undefined var --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, null given in %s on line %d
bool(false)

-- unset var --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, null given in %s on line %d
bool(false)

-- resource --

Warning: DateTime::setTimezone() expects parameter 1 to be DateTimeZone, resource given in %s on line %d
bool(false)
===DONE===

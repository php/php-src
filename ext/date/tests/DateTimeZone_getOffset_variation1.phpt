--TEST--
Test DateTimeZone::getOffset() function : usage variation - Passing unexpected values to first argument $datetime.
--FILE--
<?php
/* Prototype  : int DateTimeZone::getOffset  ( DateTime $datetime  )
 * Description: Returns the timezone offset from GMT
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_offset_get()
 */

echo "*** Testing DateTimeZone::getOffset() : usage variation -  unexpected values to first argument \$datetime***\n";

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

$timezone = new DateTimezone("Europe/London");

foreach($inputs as $variation =>$datetime) {
    echo "\n-- $variation --\n";
   	var_dump( $timezone->getOffset($datetime) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone::getOffset() : usage variation -  unexpected values to first argument $datetime***

-- int 0 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, int given in %s on line %d
bool(false)

-- int 1 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, int given in %s on line %d
bool(false)

-- int 12345 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, int given in %s on line %d
bool(false)

-- int -12345 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, int given in %s on line %d
bool(false)

-- float 10.5 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, float given in %s on line %d
bool(false)

-- float -10.5 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, float given in %s on line %d
bool(false)

-- float .5 --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, float given in %s on line %d
bool(false)

-- empty array --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, array given in %s on line %d
bool(false)

-- associative array --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, array given in %s on line %d
bool(false)

-- uppercase NULL --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, null given in %s on line %d
bool(false)

-- lowercase null --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, null given in %s on line %d
bool(false)

-- lowercase true --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, bool given in %s on line %d
bool(false)

-- lowercase false --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, bool given in %s on line %d
bool(false)

-- uppercase TRUE --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, bool given in %s on line %d
bool(false)

-- uppercase FALSE --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, bool given in %s on line %d
bool(false)

-- empty string DQ --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- string DQ --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- string SQ --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- heredoc --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, object given in %s on line %d
bool(false)

-- undefined var --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, null given in %s on line %d
bool(false)

-- unset var --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, null given in %s on line %d
bool(false)

-- resource --

Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, resource given in %s on line %d
bool(false)
===DONE===

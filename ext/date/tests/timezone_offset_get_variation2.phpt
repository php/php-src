--TEST--
Test timezone_offset_get() function : usage variation - Passing unexpected values to second argument $datetime.
--FILE--
<?php
/* Prototype  : int timezone_offset_get  ( DateTimeZone $object  , DateTime $datetime  )
 * Description: Returns the timezone offset from GMT
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getOffset()
 */
 
echo "*** Testing timezone_offset_get() : usage variation -  unexpected values to second argument \$datetime***\n";

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

$object = new DateTimezone("Europe/London");

foreach($inputs as $variation =>$datetime) {
    echo "\n-- $variation --\n";
   	var_dump( timezone_offset_get($object, $datetime) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_offset_get() : usage variation -  unexpected values to second argument $datetime***

-- int 0 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, integer given in %s on line %d
bool(false)

-- int 1 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, integer given in %s on line %d
bool(false)

-- int 12345 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, integer given in %s on line %d
bool(false)

-- int -12345 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, integer given in %s on line %d
bool(false)

-- float 10.5 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, double given in %s on line %d
bool(false)

-- float -10.5 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, double given in %s on line %d
bool(false)

-- float .5 --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, double given in %s on line %d
bool(false)

-- empty array --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, array given in %s on line %d
bool(false)

-- associative array --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, array given in %s on line %d
bool(false)

-- uppercase NULL --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, null given in %s on line %d
bool(false)

-- lowercase null --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, null given in %s on line %d
bool(false)

-- lowercase true --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, boolean given in %s on line %d
bool(false)

-- lowercase false --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, boolean given in %s on line %d
bool(false)

-- uppercase TRUE --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, boolean given in %s on line %d
bool(false)

-- uppercase FALSE --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, boolean given in %s on line %d
bool(false)

-- empty string DQ --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- string DQ --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- string SQ --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- heredoc --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, object given in %s on line %d
bool(false)

-- undefined var --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, null given in %s on line %d
bool(false)

-- unset var --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, null given in %s on line %d
bool(false)

-- resource --

Warning: timezone_offset_get() expects parameter 2 to be DateTime, resource given in %s on line %d
bool(false)
===DONE===

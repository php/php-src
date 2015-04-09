--TEST--
Test timezone_offset_get() function : usage variation - Passing unexpected values to first argument $object.
--FILE--
<?php
/* Prototype  : int timezone_offset_get  ( DateTimeZone $object  , DateTime $datetime  )
 * Description: Returns the timezone offset from GMT
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getOffset()
 */
 
echo "*** Testing timezone_offset_get() : usage variation -  unexpected values to first argument \$object***\n";

//Set the default time zone 
date_default_timezone_set("Europe/London");

set_error_handler('handler');

function handler($errno, $errstr) {
	if ($errno === E_RECOVERABLE_ERROR) {
		echo $errstr . "\n";
	}
}

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

$datetime = new DateTime("2009-01-31 15:14:10");

foreach($inputs as $variation =>$object) {
    echo "\n-- $variation --\n";
   	var_dump( timezone_offset_get($object, $datetime) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_offset_get() : usage variation -  unexpected values to first argument $object***

-- int 0 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given
bool(false)

-- int 1 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given
bool(false)

-- int 12345 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given
bool(false)

-- int -12345 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given
bool(false)

-- float 10.5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given
bool(false)

-- float -10.5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given
bool(false)

-- float .5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given
bool(false)

-- empty array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given
bool(false)

-- int indexed array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given
bool(false)

-- associative array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given
bool(false)

-- nested arrays --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given
bool(false)

-- uppercase NULL --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given
bool(false)

-- lowercase null --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given
bool(false)

-- lowercase true --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given
bool(false)

-- lowercase false --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given
bool(false)

-- uppercase TRUE --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given
bool(false)

-- uppercase FALSE --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given
bool(false)

-- empty string DQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- empty string SQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- string DQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- string SQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- mixed case string --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- heredoc --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given
bool(false)

-- instance of classWithToString --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of classWithToString given
bool(false)

-- instance of classWithoutToString --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of classWithoutToString given
bool(false)

-- undefined var --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given
bool(false)

-- unset var --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given
bool(false)

-- resource --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, resource given
bool(false)
===DONE===

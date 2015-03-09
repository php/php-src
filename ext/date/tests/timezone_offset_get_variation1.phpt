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
	try {
		var_dump( timezone_offset_get($object, $datetime) );
	} catch (EngineException $ex) {
		echo $ex->getMessage()."\n";
	}
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_offset_get() : usage variation -  unexpected values to first argument $object***

-- int 0 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given

-- int 1 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given

-- int 12345 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given

-- int -12345 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, integer given

-- float 10.5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given

-- float -10.5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given

-- float .5 --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, float given

-- empty array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given

-- int indexed array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given

-- associative array --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given

-- nested arrays --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, array given

-- uppercase NULL --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given

-- lowercase null --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given

-- lowercase true --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given

-- lowercase false --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given

-- uppercase TRUE --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given

-- uppercase FALSE --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, boolean given

-- empty string DQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- empty string SQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- string DQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- string SQ --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- mixed case string --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- heredoc --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, string given

-- instance of classWithToString --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of classWithToString given

-- instance of classWithoutToString --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, instance of classWithoutToString given

-- undefined var --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given

-- unset var --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, null given

-- resource --
Argument 1 passed to timezone_offset_get() must be an instance of DateTimeZone, resource given
===DONE===

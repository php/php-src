--TEST--
Test DateTime::__construct() function : usage variation - Passing unexpected values to first argument $timezone.
--FILE--
<?php
/* Prototype  : DateTimeZone::__construct  ( string $timezone  )
 * Description: Returns new DateTimeZone object
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing DateTime::__construct() : usage variation -  unexpected values to first argument \$timezone***\n";

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

foreach($inputs as $variation =>$timezone) {
      echo "\n-- $variation --\n";
      try {
      	var_dump( new DateTimezone($timezone) );
      } catch(Exception $e) {
      	  $msg = $e->getMessage();
      	  echo "FAILED: " . $msg . "\n";
      }	
      
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::__construct() : usage variation -  unexpected values to first argument $timezone***

-- int 0 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (0)

-- int 1 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (1)

-- int 12345 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (12345)

-- int -12345 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (-12345)

-- float 10.5 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (10.5)

-- float -10.5 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (-10.5)

-- float .5 --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (0.5)

-- empty array --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, array given

-- int indexed array --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, array given

-- associative array --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, array given

-- nested arrays --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, array given

-- uppercase NULL --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- lowercase null --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- lowercase true --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (1)

-- lowercase false --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- uppercase TRUE --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (1)

-- uppercase FALSE --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- empty string DQ --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- empty string SQ --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- string DQ --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (string)

-- string SQ --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (string)

-- mixed case string --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (sTrInG)

-- heredoc --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (hello world)

-- instance of classWithToString --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone (Class A object)

-- instance of classWithoutToString --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, object given

-- undefined var --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- unset var --
FAILED: DateTimeZone::__construct(): Unknown or bad timezone ()

-- resource --
FAILED: DateTimeZone::__construct() expects parameter 1 to be string, resource given
===DONE===

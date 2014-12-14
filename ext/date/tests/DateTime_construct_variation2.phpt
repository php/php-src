--TEST--
Test new DateTime() function : usage variation - Passing unexpected values to second argument $timezone.
--FILE--
<?php
/* Prototype  : DateTime::__construct  ([ string $time="now"  [, DateTimeZone $timezone=NULL  ]] )
 * Description: Returns new DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: date_create
 */

echo "*** Testing new DateTime() : usage variation -  unexpected values to second argument \$timezone***\n";

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

$time = "2005-07-14 22:30:41";

foreach($inputs as $variation =>$timezone) {
      echo "\n-- $variation --\n";
      
      try {
			var_dump( new DateTime($time, $timezone) );
      } catch(Exception $e) {
			$msg = $e->getMessage();
			echo "FAILED: " . $msg . "\n";
      }	
      
};

// closing the resource
fclose( $file_handle);

?>
===DONE===
--EXPECTF--
*** Testing new DateTime() : usage variation -  unexpected values to second argument $timezone***

-- int 0 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, integer given

-- int 1 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, integer given

-- int 12345 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, integer given

-- int -12345 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, integer given

-- float 10.5 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, float given

-- float -10.5 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, float given

-- float .5 --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, float given

-- empty array --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, array given

-- int indexed array --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, array given

-- associative array --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, array given

-- nested arrays --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, array given

-- uppercase NULL --
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase null --
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase true --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, boolean given

-- lowercase false --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, boolean given

-- uppercase TRUE --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, boolean given

-- uppercase FALSE --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, boolean given

-- empty string DQ --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- empty string SQ --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- string DQ --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- string SQ --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- mixed case string --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- heredoc --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, string given

-- instance of classWithToString --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, object given

-- instance of classWithoutToString --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, object given

-- undefined var --
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- unset var --
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2005-07-14 22:30:41.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- resource --
FAILED: DateTime::__construct() expects parameter 2 to be DateTimeZone, resource given
===DONE===


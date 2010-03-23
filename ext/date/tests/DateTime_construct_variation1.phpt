--TEST--
Test new DateTime() function : usage variation - Passing unexpected values to first argument $time.
--FILE--
<?php
/* Prototype  : DateTime::__construct  ([ string $time="now"  [, DateTimeZone $timezone=NULL  ]] )
 * Description: Returns new DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: date_create
 */

echo "*** Testing new DateTime(): usage variation -  unexpected values to first argument \$time***\n";

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

$timezone = new DateTimeZone("Europe/London");

foreach($inputs as $variation =>$time) {
      echo "\n-- $variation --\n";
      
      try {
      	var_dump( new DateTime($time) );
      } catch(Exception $e) {
      	  $msg = $e->getMessage();
      	  echo "FAILED: " . $msg . "\n";
      }	
      
      try {
      	var_dump( new DateTime($time, $timezone) );
      } catch(Exception$e) {
      	 $msg = $e->getMessage();
      	 echo "FAILED: " . $msg . "\n";
      }	
};

// closing the resource
fclose( $file_handle);

?>
===DONE===
--EXPECTF--
*** Testing new DateTime(): usage variation -  unexpected values to first argument $time***

-- int 0 --
FAILED: DateTime::__construct(): Failed to parse time string (0) at position 0 (0): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (0) at position 0 (0): Unexpected character

-- int 1 --
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character

-- int 12345 --
FAILED: DateTime::__construct(): Failed to parse time string (12345) at position 4 (5): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (12345) at position 4 (5): Unexpected character

-- int -12345 --
FAILED: DateTime::__construct(): Failed to parse time string (-12345) at position 5 (5): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (-12345) at position 5 (5): Unexpected character

-- float 10.5 --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- float -10.5 --
FAILED: DateTime::__construct(): Failed to parse time string (-10.5) at position 4 (5): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (-10.5) at position 4 (5): Unexpected character

-- float .5 --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty array --
FAILED: DateTime::__construct() expects parameter 1 to be string, array given
FAILED: DateTime::__construct() expects parameter 1 to be string, array given

-- int indexed array --
FAILED: DateTime::__construct() expects parameter 1 to be string, array given
FAILED: DateTime::__construct() expects parameter 1 to be string, array given

-- associative array --
FAILED: DateTime::__construct() expects parameter 1 to be string, array given
FAILED: DateTime::__construct() expects parameter 1 to be string, array given

-- nested arrays --
FAILED: DateTime::__construct() expects parameter 1 to be string, array given
FAILED: DateTime::__construct() expects parameter 1 to be string, array given

-- uppercase NULL --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase null --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase true --
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character

-- lowercase false --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- uppercase TRUE --
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character
FAILED: DateTime::__construct(): Failed to parse time string (1) at position 0 (1): Unexpected character

-- uppercase FALSE --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty string DQ --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty string SQ --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- string DQ --
FAILED: DateTime::__construct(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database
FAILED: DateTime::__construct(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database

-- string SQ --
FAILED: DateTime::__construct(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database
FAILED: DateTime::__construct(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database

-- mixed case string --
FAILED: DateTime::__construct(): Failed to parse time string (sTrInG) at position 0 (s): The timezone could not be found in the database
FAILED: DateTime::__construct(): Failed to parse time string (sTrInG) at position 0 (s): The timezone could not be found in the database

-- heredoc --
FAILED: DateTime::__construct(): Failed to parse time string (hello world) at position 0 (h): The timezone could not be found in the database
FAILED: DateTime::__construct(): Failed to parse time string (hello world) at position 0 (h): The timezone could not be found in the database

-- instance of classWithToString --
FAILED: DateTime::__construct(): Failed to parse time string (Class A object) at position 0 (C): The timezone could not be found in the database
FAILED: DateTime::__construct(): Failed to parse time string (Class A object) at position 0 (C): The timezone could not be found in the database

-- instance of classWithoutToString --
FAILED: DateTime::__construct() expects parameter 1 to be string, object given
FAILED: DateTime::__construct() expects parameter 1 to be string, object given

-- undefined var --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- unset var --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- resource --
FAILED: DateTime::__construct() expects parameter 1 to be string, resource given
FAILED: DateTime::__construct() expects parameter 1 to be string, resource given
===DONE===

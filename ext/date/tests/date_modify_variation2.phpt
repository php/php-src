--TEST--
Test date_modify() function : usage variation - Passing unexpected values to second argument $format.
--FILE--
<?php
/* Prototype  : DateTime date_modify  ( DateTime $object  , string $modify  )
 * Description: Alter the timestamp of a DateTime object by incrementing or decrementing in a format accepted by strtotime(). 
 * Source code: ext/date/php_date.c
 * Alias to functions: public DateTime DateTime::modify()
 */

echo "*** Testing date_modify() : usage variation -  unexpected values to second argument \$format***\n";

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

$object = date_create("2009-01-31 14:28:41");

foreach($inputs as $variation =>$format) {
      echo "\n-- $variation --\n";
      var_dump( date_modify($object, $format) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing date_modify() : usage variation -  unexpected values to second argument $format***

-- int 0 --

Warning: date_modify(): Failed to parse time string (0) at position 0 (0): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- int 1 --

Warning: date_modify(): Failed to parse time string (1) at position 0 (1): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- int 12345 --

Warning: date_modify(): Failed to parse time string (12345) at position 4 (5): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- int -12345 --

Warning: date_modify(): Failed to parse time string (-12345) at position 5 (5): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- float 10.5 --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "2009-01-31 10:05:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- float -10.5 --

Warning: date_modify(): Failed to parse time string (-10.5) at position 4 (5): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- float .5 --
object(DateTime)#%d (3) {
  ["date"]=>
  string(19) "2009-01-31 00:05:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty array --

Warning: date_modify() expects parameter 2 to be string, array given in %sdate_modify_variation2.php on line 99
bool(false)

-- int indexed array --

Warning: date_modify() expects parameter 2 to be string, array given in %sdate_modify_variation2.php on line 99
bool(false)

-- associative array --

Warning: date_modify() expects parameter 2 to be string, array given in %sdate_modify_variation2.php on line 99
bool(false)

-- nested arrays --

Warning: date_modify() expects parameter 2 to be string, array given in %sdate_modify_variation2.php on line 99
bool(false)

-- uppercase NULL --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- lowercase null --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- lowercase true --

Warning: date_modify(): Failed to parse time string (1) at position 0 (1): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- lowercase false --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- uppercase TRUE --

Warning: date_modify(): Failed to parse time string (1) at position 0 (1): Unexpected character in %sdate_modify_variation2.php on line 99
bool(false)

-- uppercase FALSE --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- empty string DQ --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- empty string SQ --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- string DQ --

Warning: date_modify(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database in %sdate_modify_variation2.php on line 99
bool(false)

-- string SQ --

Warning: date_modify(): Failed to parse time string (string) at position 0 (s): The timezone could not be found in the database in %sdate_modify_variation2.php on line 99
bool(false)

-- mixed case string --

Warning: date_modify(): Failed to parse time string (sTrInG) at position 0 (s): The timezone could not be found in the database in %sdate_modify_variation2.php on line 99
bool(false)

-- heredoc --

Warning: date_modify(): Failed to parse time string (hello world) at position 0 (h): The timezone could not be found in the database in %sdate_modify_variation2.php on line 99
bool(false)

-- instance of classWithToString --

Warning: date_modify(): Failed to parse time string (Class A object) at position 0 (C): The timezone could not be found in the database in %sdate_modify_variation2.php on line 99
bool(false)

-- instance of classWithoutToString --

Warning: date_modify() expects parameter 2 to be string, object given in %sdate_modify_variation2.php on line 99
bool(false)

-- undefined var --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- unset var --

Warning: date_modify(): Failed to parse time string () at position 0 ( in %sdate_modify_variation2.php on line 99
bool(false)

-- resource --

Warning: date_modify() expects parameter 2 to be string, resource given in %sdate_modify_variation2.php on line 99
bool(false)
===DONE===

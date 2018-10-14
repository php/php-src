--TEST--
Test date_format() function : usage variation - Passing unexpected values to second argument $format.
--FILE--
<?php
/* Prototype  : string date_format  ( DateTime $object  , string $format  )
 * Description: Returns date formatted according to given format
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::format
 */

echo "*** Testing date_format() : usage variation -  unexpected values to second argument \$format***\n";

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

$object = date_create("2005-07-14 22:30:41");

foreach($inputs as $variation =>$format) {
      echo "\n-- $variation --\n";
      var_dump( date_format($object, $format) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing date_format() : usage variation -  unexpected values to second argument $format***

-- int 0 --
string(1) "0"

-- int 1 --
string(1) "1"

-- int 12345 --
string(5) "12345"

-- int -12345 --
string(6) "-12345"

-- float 10.5 --
string(4) "10.5"

-- float -10.5 --
string(5) "-10.5"

-- float .5 --
string(3) "0.5"

-- empty array --

Warning: date_format() expects parameter 2 to be string, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: date_format() expects parameter 2 to be string, array given in %s on line %d
bool(false)

-- associative array --

Warning: date_format() expects parameter 2 to be string, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: date_format() expects parameter 2 to be string, array given in %s on line %d
bool(false)

-- uppercase NULL --
string(0) ""

-- lowercase null --
string(0) ""

-- lowercase true --
string(1) "1"

-- lowercase false --
string(0) ""

-- uppercase TRUE --
string(1) "1"

-- uppercase FALSE --
string(0) ""

-- empty string DQ --
string(0) ""

-- empty string SQ --
string(0) ""

-- string DQ --
string(40) "4131Thu, 14 Jul 2005 22:30:41 +010030710"

-- string SQ --
string(40) "4131Thu, 14 Jul 2005 22:30:41 +010030710"

-- mixed case string --
string(40) "41BSTThu, 14 Jul 2005 22:30:41 +01001722"

-- heredoc --
string(82) "10Europe/LondonThursdayThursday2005 42005Thu, 14 Jul 2005 22:30:41 +0100Thursday14"

-- instance of classWithToString --
string(66) "CThursdaypm4141 PM 2005b14Europe/London2005-07-14T22:30:41+01:0031"

-- instance of classWithoutToString --

Warning: date_format() expects parameter 2 to be string, object given in %s on line %d
bool(false)

-- undefined var --
string(0) ""

-- unset var --
string(0) ""

-- resource --

Warning: date_format() expects parameter 2 to be string, resource given in %s on line %d
bool(false)
===DONE===

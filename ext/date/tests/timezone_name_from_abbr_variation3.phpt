--TEST--
Test timezone_name_from_abbr() function : usage variation - Passing unexpected values to third argument $isdst.
--FILE--
<?php
/* Prototype  : string timezone_name_from_abbr  ( string $abbr  [, int $gmtOffset= -1  [, int $isdst= -1  ]] )
 * Description: Returns the timezone name from abbrevation
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing timezone_name_from_abbr() : usage variation -  unexpected values to third argument \$isdst***\n";

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

$abbr= "GMT";
$gmtOffset = 3600;

foreach($inputs as $variation =>$isdst) {
      echo "\n-- $variation --\n";
      var_dump( timezone_name_from_abbr($abbr, $gmtOffset, $isdst) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_name_from_abbr() : usage variation -  unexpected values to third argument $isdst***

-- int 0 --
string(3) "UTC"

-- int 12345 --
string(3) "UTC"

-- int -12345 --
string(3) "UTC"

-- float 10.5 --
string(3) "UTC"

-- float -10.5 --
string(3) "UTC"

-- float .5 --
string(3) "UTC"

-- empty array --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, array given in %s on line %d
bool(false)

-- associative array --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, array given in %s on line %d
bool(false)

-- uppercase NULL --
string(3) "UTC"

-- lowercase null --
string(3) "UTC"

-- lowercase true --
string(3) "UTC"

-- lowercase false --
string(3) "UTC"

-- uppercase TRUE --
string(3) "UTC"

-- uppercase FALSE --
string(3) "UTC"

-- empty string DQ --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- string DQ --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- string SQ --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- heredoc --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, object given in %s on line %d
bool(false)

-- undefined var --
string(3) "UTC"

-- unset var --
string(3) "UTC"

-- resource --

Warning: timezone_name_from_abbr() expects parameter 3 to be long, resource given in %s on line %d
bool(false)
===DONE===

--TEST--
Test timezone_name_from_abbr() function : usage variation - Passing unexpected values to first argument $abbr.
--FILE--
<?php
/* Prototype  : string timezone_name_from_abbr  ( string $abbr  [, int $gmtOffset= -1  [, int $isdst= -1  ]] )
 * Description: Returns the timezone name from abbrevation
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing timezone_name_from_abbr() : usage variation -  unexpected values to first argument \$abbr***\n";

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

$gmtOffset= 3600;
$isdst = 1;

foreach($inputs as $variation =>$abbr) {
      echo "\n-- $variation --\n";
      var_dump( timezone_name_from_abbr($abbr, $gmtOffset, $isdst) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_name_from_abbr() : usage variation -  unexpected values to first argument $abbr***

-- int 0 --
string(13) "Europe/London"

-- int 12345 --
string(13) "Europe/London"

-- int -12345 --
string(13) "Europe/London"

-- float 10.5 --
string(13) "Europe/London"

-- float -10.5 --
string(13) "Europe/London"

-- float .5 --
string(13) "Europe/London"

-- empty array --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- associative array --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- uppercase NULL --
string(13) "Europe/London"

-- lowercase null --
string(13) "Europe/London"

-- lowercase true --
string(13) "Europe/London"

-- lowercase false --
string(13) "Europe/London"

-- uppercase TRUE --
string(13) "Europe/London"

-- uppercase FALSE --
string(13) "Europe/London"

-- empty string DQ --
string(13) "Europe/London"

-- empty string SQ --
string(13) "Europe/London"

-- string DQ --
string(13) "Europe/London"

-- string SQ --
string(13) "Europe/London"

-- mixed case string --
string(13) "Europe/London"

-- heredoc --
string(13) "Europe/London"

-- instance of classWithToString --
string(13) "Europe/London"

-- instance of classWithoutToString --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, object given in %s on line %d
bool(false)

-- undefined var --
string(13) "Europe/London"

-- unset var --
string(13) "Europe/London"

-- resource --

Warning: timezone_name_from_abbr() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===DONE===

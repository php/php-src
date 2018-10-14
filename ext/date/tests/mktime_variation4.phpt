--TEST--
Test mktime() function : usage variation - Passing unexpected values to forth argument $month.
--FILE--
<?php
/* Prototype  : int mktime  ([ int $hour= date("H")  [, int $minute= date("i")  [, int $second= date("s")  [, int $month= date("n")  [, int $day= date("j")  [, int $year= date("Y")  [, int $is_dst= -1  ]]]]]]] )
 * Description: Get Unix timestamp for a date
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing mktime() : usage variation -  unexpected values to forth argument \$month***\n";

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

$hour = 10;
$minute = 30;
$second = 45;

foreach($inputs as $variation =>$month) {
      echo "\n-- $variation --\n";
      var_dump( mktime($hour, $minute, $second, $month) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing mktime() : usage variation -  unexpected values to forth argument $month***

-- int 0 --
int(%i)

-- int 12345 --
%rint\(-?[1-9][0-9]*\)|bool\(false\)%r

-- int -12345 --
%rint\(-?[1-9][0-9]*\)|bool\(false\)%r

-- float 10.5 --
int(%i)

-- float -10.5 --
int(%i)

-- float .5 --
int(%i)

-- empty array --

Warning: mktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: mktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

-- associative array --

Warning: mktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: mktime() expects parameter 4 to be integer, array given in %s on line %d
bool(false)

-- uppercase NULL --
int(%i)

-- lowercase null --
int(%i)

-- lowercase true --
int(%i)

-- lowercase false --
int(%i)

-- uppercase TRUE --
int(%i)

-- uppercase FALSE --
int(%i)

-- empty string DQ --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- string DQ --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- string SQ --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- heredoc --

Warning: mktime() expects parameter 4 to be integer, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: mktime() expects parameter 4 to be integer, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: mktime() expects parameter 4 to be integer, object given in %s on line %d
bool(false)

-- undefined var --
int(%i)

-- unset var --
int(%i)

-- resource --

Warning: mktime() expects parameter 4 to be integer, resource given in %s on line %d
bool(false)
===DONE===

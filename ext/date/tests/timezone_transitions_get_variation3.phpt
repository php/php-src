--TEST--
Test timezone_transitions_get() function : usage variation - Passing unexpected values to first argument $timestamp_env.
--FILE--
<?php
/* Prototype  : array timezone_transitions_get  ( DateTimeZone $object, [ int $timestamp_begin  [, int $timestamp_end  ]]  )
 * Description: Returns all transitions for the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getTransitions()
 */
 
echo "*** Testing timezone_transitions_get() : usage variation -  unexpected values to first argument \$timestamp_end ***\n";

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

$tz = timezone_open("Europe/London");
$timestamp_begin = mktime(0, 0, 0, 1, 1, 1975);

foreach($inputs as $variation =>$timestamp_end) {
    echo "\n-- $variation --\n";
   	$tran =  timezone_transitions_get($tz, $timestamp_begin, $timestamp_end);
   	var_dump( gettype($tran) );
	var_dump( count($tran) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing timezone_transitions_get() : usage variation -  unexpected values to first argument $timestamp_end ***

-- int 0 --
string(5) "array"
int(1)

-- int 1 --
string(5) "array"
int(1)

-- int 12345 --
string(5) "array"
int(1)

-- int -12345 --
string(5) "array"
int(1)

-- float 10.5 --
string(5) "array"
int(1)

-- float -10.5 --
string(5) "array"
int(1)

-- float .5 --
string(5) "array"
int(1)

-- empty array --

Warning: timezone_transitions_get() expects parameter 3 to be integer, array given in %s on line %d
string(7) "boolean"
int(1)

-- int indexed array --

Warning: timezone_transitions_get() expects parameter 3 to be integer, array given in %s on line %d
string(7) "boolean"
int(1)

-- associative array --

Warning: timezone_transitions_get() expects parameter 3 to be integer, array given in %s on line %d
string(7) "boolean"
int(1)

-- nested arrays --

Warning: timezone_transitions_get() expects parameter 3 to be integer, array given in %s on line %d
string(7) "boolean"
int(1)

-- uppercase NULL --
string(5) "array"
int(1)

-- lowercase null --
string(5) "array"
int(1)

-- lowercase true --
string(5) "array"
int(1)

-- lowercase false --
string(5) "array"
int(1)

-- uppercase TRUE --
string(5) "array"
int(1)

-- uppercase FALSE --
string(5) "array"
int(1)

-- empty string DQ --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- empty string SQ --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- string DQ --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- string SQ --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- mixed case string --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- heredoc --

Warning: timezone_transitions_get() expects parameter 3 to be integer, string given in %s on line %d
string(7) "boolean"
int(1)

-- instance of classWithToString --

Warning: timezone_transitions_get() expects parameter 3 to be integer, object given in %s on line %d
string(7) "boolean"
int(1)

-- instance of classWithoutToString --

Warning: timezone_transitions_get() expects parameter 3 to be integer, object given in %s on line %d
string(7) "boolean"
int(1)

-- undefined var --
string(5) "array"
int(1)

-- unset var --
string(5) "array"
int(1)

-- resource --

Warning: timezone_transitions_get() expects parameter 3 to be integer, resource given in %s on line %d
string(7) "boolean"
int(1)
===DONE===

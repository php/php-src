--TEST--
Test DateTime::modify() function : usage variation - Passing unexpected values to first argument $modify.
--FILE--
<?php
/* Prototype  : public DateTime DateTime::modify  ( string $modify  )
 * Description: Alter the timestamp of a DateTime object by incrementing or decrementing in a format accepted by strtotime(). 
 * Source code: ext/date/php_date.c
 * Alias to functions: public date_modify()
 */

echo "*** Testing DateTime::modify() : usage variation -  unexpected values to first argument \$modify***\n";

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

$object = new DateTime("2009-01-31 14:28:41");

foreach($inputs as $variation =>$format) {
      echo "\n-- $variation --\n";
      var_dump( $object->modify($format) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::modify() : usage variation -  unexpected values to first argument $modify***

-- int 0 --
NULL

-- int 1 --
NULL

-- int 12345 --
NULL

-- int -12345 --
NULL

-- float 10.5 --
NULL

-- float -10.5 --
NULL

-- float .5 --
NULL

-- empty array --

Warning: DateTime::modify() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: DateTime::modify() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- associative array --

Warning: DateTime::modify() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: DateTime::modify() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- uppercase NULL --
NULL

-- lowercase null --
NULL

-- lowercase true --
NULL

-- lowercase false --
NULL

-- uppercase TRUE --
NULL

-- uppercase FALSE --
NULL

-- empty string DQ --
NULL

-- empty string SQ --
NULL

-- string DQ --
NULL

-- string SQ --
NULL

-- mixed case string --
NULL

-- heredoc --
NULL

-- instance of classWithToString --
NULL

-- instance of classWithoutToString --

Warning: DateTime::modify() expects parameter 1 to be string, object given in %s on line %d
bool(false)

-- undefined var --
NULL

-- unset var --
NULL

-- resource --

Warning: DateTime::modify() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===DONE===

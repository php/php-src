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
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- int 1 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- int 12345 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- int -12345 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- float 10.5 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- float -10.5 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- float .5 --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

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
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase null --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase true --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- lowercase false --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- uppercase TRUE --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- uppercase FALSE --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty string DQ --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- empty string SQ --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- string DQ --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- string SQ --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- mixed case string --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- heredoc --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- instance of classWithToString --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- instance of classWithoutToString --

Warning: DateTime::modify() expects parameter 1 to be string, object given in %s on line %d
bool(false)

-- undefined var --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- unset var --
object(DateTime)#3 (3) {
  ["date"]=>
  string(19) "2009-01-31 14:28:41"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/London"
}

-- resource --

Warning: DateTime::modify() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===DONE===

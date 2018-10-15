--TEST--
Test checkdate() function : usage variation - Passing unexpected values to first argument $month.
--FILE--
<?php
/* Prototype  : bool checkdate  ( int $month  , int $day  , int $year  )
 * Description: Checks the validity of the date formed by the arguments.
 *              A date is considered valid if each parameter is properly defined.
 * Source code: ext/date/php_date.c
 */

echo "*** Testing checkdate() : usage variation -  unexpected values to first argument \$month***\n";

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

$day = 2;
$year = 1963;

foreach($inputs as $variation =>$month) {
      echo "\n-- $variation --\n";
      var_dump( checkdate($month, $day, $year) );
};

// closing the resource
fclose( $file_handle);

?>
===DONE===
--EXPECTF--
*** Testing checkdate() : usage variation -  unexpected values to first argument $month***

-- float 10.5 --
bool(true)

-- float -10.5 --
bool(false)

-- float .5 --
bool(false)

-- empty array --

Warning: checkdate() expects parameter 1 to be int, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: checkdate() expects parameter 1 to be int, array given in %s on line %d
bool(false)

-- associative array --

Warning: checkdate() expects parameter 1 to be int, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: checkdate() expects parameter 1 to be int, array given in %s on line %d
bool(false)

-- uppercase NULL --
bool(false)

-- lowercase null --
bool(false)

-- lowercase true --
bool(true)

-- lowercase false --
bool(false)

-- uppercase TRUE --
bool(true)

-- uppercase FALSE --
bool(false)

-- empty string DQ --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- empty string SQ --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- string DQ --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- string SQ --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- mixed case string --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- heredoc --

Warning: checkdate() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: checkdate() expects parameter 1 to be int, object given in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: checkdate() expects parameter 1 to be int, object given in %s on line %d
bool(false)

-- undefined var --
bool(false)

-- unset var --
bool(false)

-- resource --

Warning: checkdate() expects parameter 1 to be int, resource given in %s on line %d
bool(false)
===DONE===

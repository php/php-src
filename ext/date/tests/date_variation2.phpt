--TEST--
Test date() function : usage variation - Passing unexpected values to second argument $timestamp.
--FILE--
<?php
/* Prototype  : string date  ( string $format  [, int $timestamp  ] )
 * Description: Format a local time/date.
 * Source code: ext/date/php_date.c
 */

echo "*** Testing date() : usage variation -  unexpected values to second argument \$timestamp***\n";

//Set the default time zone 
date_default_timezone_set("Europe/London");

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define some classes
class classWithTounicode
{
	public function __tounicode() {
		return "Class A object";
	}
}

class classWithoutTounicode
{
}

// heredoc unicode
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
      'empty unicode DQ' => "",
      'empty unicode SQ' => '',

      // unicode data
      'unicode DQ' => "unicode",
      'unicode SQ' => 'unicode',
      'mixed case unicode' => "unicode",
      'heredoc' => $heredoc,

      // object data
      'instance of classWithTounicode' => new classWithTounicode(),
      'instance of classWithoutTounicode' => new classWithoutTounicode(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
      
      // resource 
      'resource' => $file_handle
);

$format = "F j, Y, g:i a";

foreach($inputs as $variation =>$timestamp) {
      echo "\n-- $variation --\n";
      var_dump( date($format, $timestamp) );
};

// closing the resource
fclose( $file_handle );

?>
===DONE===
--EXPECTF--
*** Testing date() : usage variation -  unexpected values to second argument $timestamp***

-- int 0 --
unicode(24) "January 1, 1970, 1:00 am"

-- int 1 --
unicode(24) "January 1, 1970, 1:00 am"

-- int 12345 --
unicode(24) "January 1, 1970, 4:25 am"

-- int -12345 --
unicode(26) "December 31, 1969, 9:34 pm"

-- float 10.5 --
unicode(24) "January 1, 1970, 1:00 am"

-- float -10.5 --
unicode(25) "January 1, 1970, 12:59 am"

-- float .5 --
unicode(24) "January 1, 1970, 1:00 am"

-- empty array --

Warning: date() expects parameter 2 to be long, array given in %s on line %d
bool(false)

-- int indexed array --

Warning: date() expects parameter 2 to be long, array given in %s on line %d
bool(false)

-- associative array --

Warning: date() expects parameter 2 to be long, array given in %s on line %d
bool(false)

-- nested arrays --

Warning: date() expects parameter 2 to be long, array given in %s on line %d
bool(false)

-- uppercase NULL --
unicode(24) "January 1, 1970, 1:00 am"

-- lowercase null --
unicode(24) "January 1, 1970, 1:00 am"

-- lowercase true --
unicode(24) "January 1, 1970, 1:00 am"

-- lowercase false --
unicode(24) "January 1, 1970, 1:00 am"

-- uppercase TRUE --
unicode(24) "January 1, 1970, 1:00 am"

-- uppercase FALSE --
unicode(24) "January 1, 1970, 1:00 am"

-- empty unicode DQ --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- empty unicode SQ --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- unicode DQ --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- unicode SQ --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- mixed case unicode --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- heredoc --

Warning: date() expects parameter 2 to be long, Unicode string given in %s on line %d
bool(false)

-- instance of classWithTounicode --

Warning: date() expects parameter 2 to be long, object given in %s on line %d
bool(false)

-- instance of classWithoutTounicode --

Warning: date() expects parameter 2 to be long, object given in %s on line %d
bool(false)

-- undefined var --
unicode(24) "January 1, 1970, 1:00 am"

-- unset var --
unicode(24) "January 1, 1970, 1:00 am"

-- resource --

Warning: date() expects parameter 2 to be long, resource given in %s on line %d
bool(false)
===DONE===


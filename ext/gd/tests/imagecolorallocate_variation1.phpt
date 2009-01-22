--TEST--
Test imagecolorallocate() function : usage variations  - passing different data types to first argument
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
?> 
--FILE--
<?php
/* Prototype  : int imagecolorallocate(resource im, int red, int green, int blue)
 * Description: Allocate a color for an image
 * Source code: ext/gd/gd.c
 */

echo "*** Testing imagecolorallocate() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$red = 10;
$green = 10;
$blue = 10;

$fp = tmpfile();

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

//array of values to iterate over
$values = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -12345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 10.1234567e10' => 10.1234567e10,
      'float 10.7654321E-10' => 10.7654321E-10,
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
      
      //resource 
      "file resource" => $fp
);

// loop through each element of the array for im
foreach($values as $key => $value) {
      echo "\n-- $key --\n";
      var_dump( imagecolorallocate($value, $red, $green, $blue) );
};
?>
===DONE===
--EXPECTF--
*** Testing imagecolorallocate() : usage variations ***

-- int 0 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- int 1 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- int 12345 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- int -12345 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- float 10.5 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- float -10.5 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- float 10.1234567e10 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- float 10.7654321E-10 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- float .5 --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- empty array --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- int indexed array --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- associative array --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- nested arrays --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- uppercase NULL --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- lowercase null --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- lowercase true --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- lowercase false --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- uppercase TRUE --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- uppercase FALSE --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- empty string DQ --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- empty string SQ --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- string DQ --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- string SQ --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- mixed case string --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- heredoc --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- instance of classWithToString --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- instance of classWithoutToString --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- undefined var --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- unset var --

Warning: imagecolorallocate(): supplied argument is not a valid Image resource in %s on line %d
bool(false)

-- file resource --

Warning: imagecolorallocate(): supplied resource is not a valid Image resource in %s on line %d
bool(false)
===DONE===

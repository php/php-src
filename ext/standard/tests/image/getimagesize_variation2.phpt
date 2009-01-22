--TEST--
Test getimagesize() function : usage variations  - unexpected type for arg 2
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_JPEG")) {
		die("skip jpeg format is not available");
	}
?>
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array 
 * Source code: ext/standard/image.c
 * Alias to functions: 
 */

function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	echo "Error: $err_no - $err_msg, $filename($linenum)\n";
}
set_error_handler('test_error_handler');

echo "*** Testing getimagesize() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$imagefile = dirname(__FILE__)."/test1pix.jpg";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // int data
      "0" => 0,
      "1" => 1,
      "12345" => 12345,
      "-2345" => -2345,

      // float data
      "10.5" => 10.5,
      "-10.5" => -10.5,
      "10.1234567e5" => 10.1234567e10,
      "10.7654321e-5" => 10.7654321E-5,
      .5,

      // array data
      "array()" => array(),
      "array(0)" => array(0),
      "array(1)" => array(1),
      "array(1, 2)" => array(1, 2),
      "array('color' => 'red', 'item' => 'pen')" => array('color' => 'red', 'item' => 'pen'),

      // null data
      "NULL" => NULL,
      "null" => null,

      // boolean data
      "true" => true,
      "false" => false,
      "TRUE" => TRUE,
      "FALSE" => FALSE,

      // empty data
      "\"\"" => "",
      "''" => '',

      // object data
      "new stdclass()" => new stdclass(),

      // undefined data
      "undefined_var" => $undefined_var,

      // unset data
      "unset_var" => $unset_var,
);

// loop through each element of the array for info

foreach($values as $key => $value) {
      echo "\n-- Arg value $key --\n";
      getimagesize($imagefile, $value);
      var_dump(bin2hex((binary)$value["APP0"]));
};

?>
===DONE===
--EXPECTF--
*** Testing getimagesize() : usage variations ***
Error: 8 - Undefined variable: undefined_var, %s(%d)
Error: 8 - Undefined variable: unset_var, %s(%d)

-- Arg value 0 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 1 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 12345 --
unicode(28) "4a46494600010201006000600000"

-- Arg value -2345 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 10.5 --
unicode(28) "4a46494600010201006000600000"

-- Arg value -10.5 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 10.1234567e5 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 10.7654321e-5 --
unicode(28) "4a46494600010201006000600000"

-- Arg value 12346 --
unicode(28) "4a46494600010201006000600000"

-- Arg value array() --
unicode(28) "4a46494600010201006000600000"

-- Arg value array(0) --
unicode(28) "4a46494600010201006000600000"

-- Arg value array(1) --
unicode(28) "4a46494600010201006000600000"

-- Arg value array(1, 2) --
unicode(28) "4a46494600010201006000600000"

-- Arg value array('color' => 'red', 'item' => 'pen') --
unicode(28) "4a46494600010201006000600000"

-- Arg value NULL --
unicode(28) "4a46494600010201006000600000"

-- Arg value null --
unicode(28) "4a46494600010201006000600000"

-- Arg value true --
unicode(28) "4a46494600010201006000600000"

-- Arg value false --
unicode(28) "4a46494600010201006000600000"

-- Arg value TRUE --
unicode(28) "4a46494600010201006000600000"

-- Arg value FALSE --
unicode(28) "4a46494600010201006000600000"

-- Arg value "" --
unicode(28) "4a46494600010201006000600000"

-- Arg value '' --
unicode(28) "4a46494600010201006000600000"

-- Arg value new stdclass() --
unicode(28) "4a46494600010201006000600000"

-- Arg value undefined_var --
unicode(28) "4a46494600010201006000600000"

-- Arg value unset_var --
unicode(28) "4a46494600010201006000600000"
===DONE===
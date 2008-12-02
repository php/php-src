--TEST--
Test uksort() function : usage variation 
--FILE--
<?php
/* Prototype  : bool uksort(array array_arg, string cmp_function)
 * Description: Sort an array by keys using a user-defined comparison function 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing uksort() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array_arg = array(1, 2);

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
$inputs = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -2345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
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

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,
);

// loop through each element of the array for cmp_function

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( uksort($array_arg, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing uksort() : usage variation ***

--int 0--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--int 1--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--int 12345--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--int -12345--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--float 10.5--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--float -10.5--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--float 12.3456789000e10--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--float -12.3456789000e10--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--float .5--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--empty array--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--int indexed array--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--associative array--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--nested arrays--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--uppercase NULL--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--lowercase null--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--lowercase true--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--lowercase false--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--uppercase TRUE--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--uppercase FALSE--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--empty string DQ--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--empty string SQ--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--instance of classWithToString--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--instance of classWithoutToString--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--undefined var--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)

--unset var--

Warning: uksort(): Invalid comparison function in %s on line %d
bool(false)
===DONE===


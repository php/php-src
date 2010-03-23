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

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--int 1--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--int 12345--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--int -12345--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--float 10.5--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--float -10.5--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--float 12.3456789000e10--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--float -12.3456789000e10--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--float .5--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--empty array--

Warning: uksort() expects parameter 2 to be a valid callback, array must have exactly two members in %suksort_variation2.php on line %d
NULL

--int indexed array--

Warning: uksort() expects parameter 2 to be a valid callback, array must have exactly two members in %suksort_variation2.php on line %d
NULL

--associative array--

Warning: uksort() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %suksort_variation2.php on line %d
NULL

--nested arrays--

Warning: uksort() expects parameter 2 to be a valid callback, array must have exactly two members in %suksort_variation2.php on line %d
NULL

--uppercase NULL--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--lowercase null--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--lowercase true--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--lowercase false--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--uppercase TRUE--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--uppercase FALSE--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--empty string DQ--

Warning: uksort() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %suksort_variation2.php on line %d
NULL

--empty string SQ--

Warning: uksort() expects parameter 2 to be a valid callback, function '' not found or invalid function name in %suksort_variation2.php on line %d
NULL

--instance of classWithToString--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--instance of classWithoutToString--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--undefined var--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL

--unset var--

Warning: uksort() expects parameter 2 to be a valid callback, no array or string given in %suksort_variation2.php on line %d
NULL
===DONE===

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
$cmp_function = 'string_val';

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
);

// loop through each element of the array for array_arg

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( uksort($value, $cmp_function) );
};

?>
===DONE===
--EXPECTF--
*** Testing uksort() : usage variation ***

--int 0--

Warning: uksort() expects parameter 1 to be array, integer given in %suksort_variation1.php on line %d
NULL

--int 1--

Warning: uksort() expects parameter 1 to be array, integer given in %suksort_variation1.php on line %d
NULL

--int 12345--

Warning: uksort() expects parameter 1 to be array, integer given in %suksort_variation1.php on line %d
NULL

--int -12345--

Warning: uksort() expects parameter 1 to be array, integer given in %suksort_variation1.php on line %d
NULL

--float 10.5--

Warning: uksort() expects parameter 1 to be array, float given in %suksort_variation1.php on line %d
NULL

--float -10.5--

Warning: uksort() expects parameter 1 to be array, float given in %suksort_variation1.php on line %d
NULL

--float 12.3456789000e10--

Warning: uksort() expects parameter 1 to be array, float given in %suksort_variation1.php on line %d
NULL

--float -12.3456789000e10--

Warning: uksort() expects parameter 1 to be array, float given in %suksort_variation1.php on line %d
NULL

--float .5--

Warning: uksort() expects parameter 1 to be array, float given in %suksort_variation1.php on line %d
NULL

--uppercase NULL--

Warning: uksort() expects parameter 1 to be array, null given in %suksort_variation1.php on line %d
NULL

--lowercase null--

Warning: uksort() expects parameter 1 to be array, null given in %suksort_variation1.php on line %d
NULL

--lowercase true--

Warning: uksort() expects parameter 1 to be array, boolean given in %suksort_variation1.php on line %d
NULL

--lowercase false--

Warning: uksort() expects parameter 1 to be array, boolean given in %suksort_variation1.php on line %d
NULL

--uppercase TRUE--

Warning: uksort() expects parameter 1 to be array, boolean given in %suksort_variation1.php on line %d
NULL

--uppercase FALSE--

Warning: uksort() expects parameter 1 to be array, boolean given in %suksort_variation1.php on line %d
NULL

--empty string DQ--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--empty string SQ--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--string DQ--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--string SQ--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--mixed case string--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--heredoc--

Warning: uksort() expects parameter 1 to be array, string given in %suksort_variation1.php on line %d
NULL

--instance of classWithToString--

Warning: uksort() expects parameter 1 to be array, object given in %suksort_variation1.php on line %d
NULL

--instance of classWithoutToString--

Warning: uksort() expects parameter 1 to be array, object given in %suksort_variation1.php on line %d
NULL

--undefined var--

Warning: uksort() expects parameter 1 to be array, null given in %suksort_variation1.php on line %d
NULL

--unset var--

Warning: uksort() expects parameter 1 to be array, null given in %suksort_variation1.php on line %d
NULL
===DONE===

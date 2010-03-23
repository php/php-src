--TEST--
Test array_product() function : usage variation 
--FILE--
<?php
/* Prototype  : mixed array_product(array input)
 * Description: Returns the product of the array entries 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_product() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)

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

// loop through each element of the array for input

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_product($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing array_product() : usage variation ***

--int 0--

Warning: array_product() expects parameter 1 to be array, integer given in %sarray_product_variation5.php on line %d
NULL

--int 1--

Warning: array_product() expects parameter 1 to be array, integer given in %sarray_product_variation5.php on line %d
NULL

--int 12345--

Warning: array_product() expects parameter 1 to be array, integer given in %sarray_product_variation5.php on line %d
NULL

--int -12345--

Warning: array_product() expects parameter 1 to be array, integer given in %sarray_product_variation5.php on line %d
NULL

--float 10.5--

Warning: array_product() expects parameter 1 to be array, double given in %sarray_product_variation5.php on line %d
NULL

--float -10.5--

Warning: array_product() expects parameter 1 to be array, double given in %sarray_product_variation5.php on line %d
NULL

--float 12.3456789000e10--

Warning: array_product() expects parameter 1 to be array, double given in %sarray_product_variation5.php on line %d
NULL

--float -12.3456789000e10--

Warning: array_product() expects parameter 1 to be array, double given in %sarray_product_variation5.php on line %d
NULL

--float .5--

Warning: array_product() expects parameter 1 to be array, double given in %sarray_product_variation5.php on line %d
NULL

--uppercase NULL--

Warning: array_product() expects parameter 1 to be array, null given in %sarray_product_variation5.php on line %d
NULL

--lowercase null--

Warning: array_product() expects parameter 1 to be array, null given in %sarray_product_variation5.php on line %d
NULL

--lowercase true--

Warning: array_product() expects parameter 1 to be array, boolean given in %sarray_product_variation5.php on line %d
NULL

--lowercase false--

Warning: array_product() expects parameter 1 to be array, boolean given in %sarray_product_variation5.php on line %d
NULL

--uppercase TRUE--

Warning: array_product() expects parameter 1 to be array, boolean given in %sarray_product_variation5.php on line %d
NULL

--uppercase FALSE--

Warning: array_product() expects parameter 1 to be array, boolean given in %sarray_product_variation5.php on line %d
NULL

--empty string DQ--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--empty string SQ--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--string DQ--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--string SQ--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--mixed case string--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--heredoc--

Warning: array_product() expects parameter 1 to be array, string given in %sarray_product_variation5.php on line %d
NULL

--instance of classWithToString--

Warning: array_product() expects parameter 1 to be array, object given in %sarray_product_variation5.php on line %d
NULL

--instance of classWithoutToString--

Warning: array_product() expects parameter 1 to be array, object given in %sarray_product_variation5.php on line %d
NULL

--undefined var--

Warning: array_product() expects parameter 1 to be array, null given in %sarray_product_variation5.php on line %d
NULL

--unset var--

Warning: array_product() expects parameter 1 to be array, null given in %sarray_product_variation5.php on line %d
NULL
===DONE===

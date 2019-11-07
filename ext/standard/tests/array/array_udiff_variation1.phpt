--TEST--
Test array_udiff() function : usage variation
--FILE--
<?php
/* Prototype  : array array_udiff(array arr1, array arr2 [, array ...], callback data_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments. Elements are compared by user supplied function.
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_udiff() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$arr2 = array(1, 2);

include('compare_function.inc');
$data_comp_func = 'compare_function';

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

// loop through each element of the array for arr1

foreach($inputs as $key =>$value) {
    echo "\n--$key--\n";
    try {
        var_dump( array_udiff($value, $arr2, $data_comp_func) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
};

?>
--EXPECT--
*** Testing array_udiff() : usage variation ***

--int 0--
Expected parameter 1 to be an array, int given

--int 1--
Expected parameter 1 to be an array, int given

--int 12345--
Expected parameter 1 to be an array, int given

--int -12345--
Expected parameter 1 to be an array, int given

--float 10.5--
Expected parameter 1 to be an array, float given

--float -10.5--
Expected parameter 1 to be an array, float given

--float 12.3456789000e10--
Expected parameter 1 to be an array, float given

--float -12.3456789000e10--
Expected parameter 1 to be an array, float given

--float .5--
Expected parameter 1 to be an array, float given

--uppercase NULL--
Expected parameter 1 to be an array, null given

--lowercase null--
Expected parameter 1 to be an array, null given

--lowercase true--
Expected parameter 1 to be an array, bool given

--lowercase false--
Expected parameter 1 to be an array, bool given

--uppercase TRUE--
Expected parameter 1 to be an array, bool given

--uppercase FALSE--
Expected parameter 1 to be an array, bool given

--empty string DQ--
Expected parameter 1 to be an array, string given

--empty string SQ--
Expected parameter 1 to be an array, string given

--string DQ--
Expected parameter 1 to be an array, string given

--string SQ--
Expected parameter 1 to be an array, string given

--mixed case string--
Expected parameter 1 to be an array, string given

--heredoc--
Expected parameter 1 to be an array, string given

--instance of classWithToString--
Expected parameter 1 to be an array, object given

--instance of classWithoutToString--
Expected parameter 1 to be an array, object given

--undefined var--
Expected parameter 1 to be an array, null given

--unset var--
Expected parameter 1 to be an array, null given

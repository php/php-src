--TEST--
Test array_intersect_uassoc() function : usage variation - Passing unexpected values to second argument
--FILE--
<?php
/* Prototype  : array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays with additional index check, compares indexes by a callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

// Initialise function arguments
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array3 = array("a"=>"green", "brown");

//Callback function
function key_compare_func($a, $b) {
    if ($a === $b) {
        return 0;
    }
    return ($a > $b) ? 1 : -1;
}

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//resource variable
$fp = fopen(__FILE__, "r");

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
      'int -12345' => -12345,

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

      // resource data
      'resource' => $fp,
);

// loop through each element of the array for arr1
foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_intersect_uassoc($array1, $value, 'key_compare_func') );
      var_dump( array_intersect_uassoc($array1, $value, $array3, 'key_compare_func') );
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : usage variation ***

--int 0--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--int 1--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--int 12345--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--int -12345--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--float 10.5--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--float -10.5--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--float 12.3456789000e10--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--float -12.3456789000e10--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--float .5--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--uppercase NULL--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--lowercase null--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--lowercase true--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--lowercase false--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--uppercase TRUE--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--uppercase FALSE--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--empty string DQ--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--empty string SQ--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--string DQ--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--string SQ--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--mixed case string--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--heredoc--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--instance of classWithToString--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--instance of classWithoutToString--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--undefined var--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--unset var--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

--resource--

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL

Warning: array_intersect_uassoc(): Argument #2 is not an array in %s on line %d
NULL
===DONE===

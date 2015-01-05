--TEST--
Test array_diff_ukey() function : usage variation - Passing unexpected values to callback argument
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);
$array2 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array3 = array(1, 2, 3, 4, 5);

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

// loop through each element of the array for ...

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( array_diff_ukey($array1, $array2, $value) );
      var_dump( array_diff_ukey($array1, $array2, $array3, $value) );
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***

--int 0--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--int 1--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--int 12345--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--int -12345--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--float 10.5--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--float -10.5--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--float 12.3456789000e10--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--float -12.3456789000e10--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--float .5--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--uppercase NULL--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--lowercase null--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--lowercase true--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--lowercase false--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--uppercase TRUE--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--uppercase FALSE--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--instance of classWithToString--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--instance of classWithoutToString--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--undefined var--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--unset var--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

--resource--

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL
===DONE===

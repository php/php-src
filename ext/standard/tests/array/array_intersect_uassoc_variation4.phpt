--TEST--
Test array_intersect_uassoc() function : usage variation - Passing unexpected values to mandatory third argument
--FILE--
<?php
/* Prototype  : array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays with additional index check, compares indexes by a callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

// Initialise function arguments 
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
$array3 = array("a"=>"green", "brown");

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

      // object data
      'instance of classWithToString' => new classWithToString(),

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
      var_dump( array_intersect_uassoc($array1, $array2, $value) );
      var_dump( array_intersect_uassoc($array1, $array2, $array3, $value) );
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : usage variation ***

--int 0--

Warning: array_intersect_uassoc(): Not a valid callback 0 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 0 in %s on line %d
NULL

--int 1--

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

--int 12345--

Warning: array_intersect_uassoc(): Not a valid callback 12345 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 12345 in %s on line %d
NULL

--int -12345--

Warning: array_intersect_uassoc(): Not a valid callback -12345 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback -12345 in %s on line %d
NULL

--float 10.5--

Warning: array_intersect_uassoc(): Not a valid callback 10.5 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 10.5 in %s on line %d
NULL

--float -10.5--

Warning: array_intersect_uassoc(): Not a valid callback -10.5 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback -10.5 in %s on line %d
NULL

--float 12.3456789000e10--

Warning: array_intersect_uassoc(): Not a valid callback 123456789000 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 123456789000 in %s on line %d
NULL

--float -12.3456789000e10--

Warning: array_intersect_uassoc(): Not a valid callback -123456789000 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback -123456789000 in %s on line %d
NULL

--float .5--

Warning: array_intersect_uassoc(): Not a valid callback 0.5 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 0.5 in %s on line %d
NULL

--uppercase NULL--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--lowercase null--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--lowercase true--

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

--lowercase false--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--uppercase TRUE--

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback 1 in %s on line %d
NULL

--uppercase FALSE--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--instance of classWithToString--

Warning: array_intersect_uassoc(): Not a valid callback Class A object in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback Class A object in %s on line %d
NULL

--undefined var--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--unset var--

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback  in %s on line %d
NULL

--resource--

Warning: array_intersect_uassoc(): Not a valid callback Resource id #5 in %s on line %d
NULL

Warning: array_intersect_uassoc(): Not a valid callback Resource id #5 in %s on line %d
NULL
===DONE===

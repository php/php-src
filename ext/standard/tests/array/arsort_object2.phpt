--TEST--
Test arsort() function : object functionality - sorting objects with diff. accessibility of member vars 
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $asort_flags] )
 * Description: Sort an array and maintain index association.  
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing arsort() by providing integer/string object arrays with following flag values 
 * 1. Defualt flag value
   2. SORT_REGULAR - compare items normally
*/

echo "*** Testing arsort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_arsort
{
  public $public_class_value;
  private $private_class_value;
  protected $protected_class_value; 
  // initializing object member value
  function __construct($value1, $value2,$value3){
    $this->public_class_value = $value1;
    $this->private_class_value = $value2;
    $this->protected_class_value = $value3;
  }

}

// class declaration for string objects
class for_string_arsort
{
  public $public_class_value;
  private $private_class_value;
  protected $protected_class_value; 
  // initializing object member value
  function __construct($value1, $value2,$value3){
    $this->public_class_value = $value1;
    $this->private_class_value = $value2;
    $this->protected_class_value = $value3;
  }

  // return string value
  function __tostring() {
   return (string)$this->value;
  }
}

// array of integer objects
$unsorted_int_obj = array ( 
  1 => new for_integer_arsort(11, 33,2), 2 =>  new for_integer_asort(44, 66,3),
  3 => new for_integer_arsort(23, 32,6), 4 => new for_integer_asort(-88, -5,-4),
);

// array of string objects
$unsorted_str_obj = array ( 
  "a" => new for_string_arsort("axx","AXX","d"), "b" => new for_string_asort("T", "t","q"),
  "c" => new for_string_arsort("w", "W","c"), "d" => new for_string_asort("PY", "py","s"),
);


echo "\n-- Testing arsort() by supplying various object arrays, 'flag' value is default --\n";

// testing arsort() function by supplying integer object array, flag value is default
$temp_array = $unsorted_int_obj;
var_dump(arsort($temp_array) );
var_dump($temp_array);

// testing arsort() function by supplying string object array, flag value is default
$temp_array = $unsorted_str_obj;
var_dump(arsort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing arsort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(arsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing arsort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(arsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing arsort() : object functionality ***

Fatal error: Class 'for_integer_asort' not found in %sarsort_object2.php on line %d
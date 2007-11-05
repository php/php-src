--TEST--
Test sort() function : object functionality - sorting objects with diff. accessibility of member vars
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array. 
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing sort() by providing integer/string object arrays with flag values are defualt, SORT_REGULAR
*/

echo "*** Testing sort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_sort
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
class for_string_sort
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
$unsorted_int_obj = array( 
  new for_integer_sort(11,33,30),
  new for_integer_sort(66,44,4),
  new for_integer_sort(-88,-5,5),
  new for_integer_sort(0.001,99.5,0.1)
);

// array of string objects
$unsorted_str_obj = array ( 
  new for_string_sort("axx","AXX","ass"), 
  new for_string_sort("t","eee","abb"),
  new for_string_sort("w","W", "c"),
  new for_string_sort("py","PY", "pt"),
);


echo "\n-- Testing sort() by supplying various object arrays, 'flag' value is defualt --\n";

// testing sort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(sort($temp_array) );
var_dump($temp_array);

// testing sort() function by supplying string object array, flag value is defualt
$temp_array = $unsorted_str_obj;
var_dump(sort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing sort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing sort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(sort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing sort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(sort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : object functionality ***

-- Testing sort() by supplying various object arrays, 'flag' value is defualt --
bool(true)
array(4) {
  [0]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value:private"]=>
    int(-5)
    ["protected_class_value:protected"]=>
    int(5)
  }
  [1]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    float(0.001)
    ["private_class_value:private"]=>
    float(99.5)
    ["protected_class_value:protected"]=>
    float(0.1)
  }
  [2]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value:private"]=>
    int(33)
    ["protected_class_value:protected"]=>
    int(30)
  }
  [3]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(66)
    ["private_class_value:private"]=>
    int(44)
    ["protected_class_value:protected"]=>
    int(4)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value:private"]=>
    string(3) "AXX"
    ["protected_class_value:protected"]=>
    string(3) "ass"
  }
  [1]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(2) "py"
    ["private_class_value:private"]=>
    string(2) "PY"
    ["protected_class_value:protected"]=>
    string(2) "pt"
  }
  [2]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(1) "t"
    ["private_class_value:private"]=>
    string(3) "eee"
    ["protected_class_value:protected"]=>
    string(3) "abb"
  }
  [3]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value:private"]=>
    string(1) "W"
    ["protected_class_value:protected"]=>
    string(1) "c"
  }
}

-- Testing sort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value:private"]=>
    int(-5)
    ["protected_class_value:protected"]=>
    int(5)
  }
  [1]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    float(0.001)
    ["private_class_value:private"]=>
    float(99.5)
    ["protected_class_value:protected"]=>
    float(0.1)
  }
  [2]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value:private"]=>
    int(33)
    ["protected_class_value:protected"]=>
    int(30)
  }
  [3]=>
  object(for_integer_sort)#%d (3) {
    ["public_class_value"]=>
    int(66)
    ["private_class_value:private"]=>
    int(44)
    ["protected_class_value:protected"]=>
    int(4)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value:private"]=>
    string(3) "AXX"
    ["protected_class_value:protected"]=>
    string(3) "ass"
  }
  [1]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(2) "py"
    ["private_class_value:private"]=>
    string(2) "PY"
    ["protected_class_value:protected"]=>
    string(2) "pt"
  }
  [2]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(1) "t"
    ["private_class_value:private"]=>
    string(3) "eee"
    ["protected_class_value:protected"]=>
    string(3) "abb"
  }
  [3]=>
  object(for_string_sort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value:private"]=>
    string(1) "W"
    ["protected_class_value:protected"]=>
    string(1) "c"
  }
}
Done

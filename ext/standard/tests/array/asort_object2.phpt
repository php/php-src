--TEST--
Test asort() function : object functionality - sorting objects with diff. accessibility of member vars 
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $asort_flags] )
 * Description: Sort an array and maintain index association.  
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing asort() by providing integer/string object arrays with following flag values 
 * 1. Defualt flag value
   2. SORT_REGULAR - compare items normally
*/

echo "*** Testing asort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_asort
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
class for_string_asort
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
  1 => new for_integer_asort(11, 33,2), 2 =>  new for_integer_asort(44, 66,3),
  3 => new for_integer_asort(23, 32,6), 4 => new for_integer_asort(-88, -5,-4),
);

// array of string objects
$unsorted_str_obj = array ( 
  "a" => new for_string_asort("axx","AXX","d"), "b" => new for_string_asort("T", "t","q"),
  "c" => new for_string_asort("w", "W","c"), "d" => new for_string_asort("PY", "py","s"),
);


echo "\n-- Testing asort() by supplying various object arrays, 'flag' value is default --\n";

// testing asort() function by supplying integer object array, flag value is default
$temp_array = $unsorted_int_obj;
var_dump(asort($temp_array) );
var_dump($temp_array);

// testing asort() function by supplying string object array, flag value is default
$temp_array = $unsorted_str_obj;
var_dump(asort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing asort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing asort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(asort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing asort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(asort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing asort() : object functionality ***

-- Testing asort() by supplying various object arrays, 'flag' value is default --
bool(true)
array(4) {
  [4]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value:private"]=>
    int(-5)
    ["protected_class_value:protected"]=>
    int(-4)
  }
  [1]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value:private"]=>
    int(33)
    ["protected_class_value:protected"]=>
    int(2)
  }
  [3]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(23)
    ["private_class_value:private"]=>
    int(32)
    ["protected_class_value:protected"]=>
    int(6)
  }
  [2]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(44)
    ["private_class_value:private"]=>
    int(66)
    ["protected_class_value:protected"]=>
    int(3)
  }
}
bool(true)
array(4) {
  ["d"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(2) "PY"
    ["private_class_value:private"]=>
    string(2) "py"
    ["protected_class_value:protected"]=>
    string(1) "s"
  }
  ["b"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(1) "T"
    ["private_class_value:private"]=>
    string(1) "t"
    ["protected_class_value:protected"]=>
    string(1) "q"
  }
  ["a"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value:private"]=>
    string(3) "AXX"
    ["protected_class_value:protected"]=>
    string(1) "d"
  }
  ["c"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value:private"]=>
    string(1) "W"
    ["protected_class_value:protected"]=>
    string(1) "c"
  }
}

-- Testing asort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [4]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value:private"]=>
    int(-5)
    ["protected_class_value:protected"]=>
    int(-4)
  }
  [1]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value:private"]=>
    int(33)
    ["protected_class_value:protected"]=>
    int(2)
  }
  [3]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(23)
    ["private_class_value:private"]=>
    int(32)
    ["protected_class_value:protected"]=>
    int(6)
  }
  [2]=>
  object(for_integer_asort)#%d (3) {
    ["public_class_value"]=>
    int(44)
    ["private_class_value:private"]=>
    int(66)
    ["protected_class_value:protected"]=>
    int(3)
  }
}
bool(true)
array(4) {
  ["d"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(2) "PY"
    ["private_class_value:private"]=>
    string(2) "py"
    ["protected_class_value:protected"]=>
    string(1) "s"
  }
  ["b"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(1) "T"
    ["private_class_value:private"]=>
    string(1) "t"
    ["protected_class_value:protected"]=>
    string(1) "q"
  }
  ["a"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value:private"]=>
    string(3) "AXX"
    ["protected_class_value:protected"]=>
    string(1) "d"
  }
  ["c"]=>
  object(for_string_asort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value:private"]=>
    string(1) "W"
    ["protected_class_value:protected"]=>
    string(1) "c"
  }
}
Done

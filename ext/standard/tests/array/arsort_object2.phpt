--TEST--
Test arsort() function : object functionality - sorting objects with diff. accessibility of member vars
--FILE--
<?php
/*
 * testing arsort() by providing integer/string object arrays with following flag values
 * 1. Default flag value
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
  1 => new for_integer_arsort(11, 33,2), 2 =>  new for_integer_arsort(44, 66,3),
  3 => new for_integer_arsort(23, 32,6), 4 => new for_integer_arsort(-88, -5,-4),
);

// array of string objects
$unsorted_str_obj = array (
  "a" => new for_string_arsort("axx","AXX","d"), "b" => new for_string_arsort("T", "t","q"),
  "c" => new for_string_arsort("w", "W","c"), "d" => new for_string_arsort("PY", "py","s"),
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
--EXPECT--
*** Testing arsort() : object functionality ***

-- Testing arsort() by supplying various object arrays, 'flag' value is default --
bool(true)
array(4) {
  [2]=>
  object(for_integer_arsort)#2 (3) {
    ["public_class_value"]=>
    int(44)
    ["private_class_value":"for_integer_arsort":private]=>
    int(66)
    ["protected_class_value":protected]=>
    int(3)
  }
  [3]=>
  object(for_integer_arsort)#3 (3) {
    ["public_class_value"]=>
    int(23)
    ["private_class_value":"for_integer_arsort":private]=>
    int(32)
    ["protected_class_value":protected]=>
    int(6)
  }
  [1]=>
  object(for_integer_arsort)#1 (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value":"for_integer_arsort":private]=>
    int(33)
    ["protected_class_value":protected]=>
    int(2)
  }
  [4]=>
  object(for_integer_arsort)#4 (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value":"for_integer_arsort":private]=>
    int(-5)
    ["protected_class_value":protected]=>
    int(-4)
  }
}
bool(true)
array(4) {
  ["c"]=>
  object(for_string_arsort)#7 (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value":"for_string_arsort":private]=>
    string(1) "W"
    ["protected_class_value":protected]=>
    string(1) "c"
  }
  ["a"]=>
  object(for_string_arsort)#5 (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value":"for_string_arsort":private]=>
    string(3) "AXX"
    ["protected_class_value":protected]=>
    string(1) "d"
  }
  ["b"]=>
  object(for_string_arsort)#6 (3) {
    ["public_class_value"]=>
    string(1) "T"
    ["private_class_value":"for_string_arsort":private]=>
    string(1) "t"
    ["protected_class_value":protected]=>
    string(1) "q"
  }
  ["d"]=>
  object(for_string_arsort)#8 (3) {
    ["public_class_value"]=>
    string(2) "PY"
    ["private_class_value":"for_string_arsort":private]=>
    string(2) "py"
    ["protected_class_value":protected]=>
    string(1) "s"
  }
}

-- Testing arsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [2]=>
  object(for_integer_arsort)#2 (3) {
    ["public_class_value"]=>
    int(44)
    ["private_class_value":"for_integer_arsort":private]=>
    int(66)
    ["protected_class_value":protected]=>
    int(3)
  }
  [3]=>
  object(for_integer_arsort)#3 (3) {
    ["public_class_value"]=>
    int(23)
    ["private_class_value":"for_integer_arsort":private]=>
    int(32)
    ["protected_class_value":protected]=>
    int(6)
  }
  [1]=>
  object(for_integer_arsort)#1 (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value":"for_integer_arsort":private]=>
    int(33)
    ["protected_class_value":protected]=>
    int(2)
  }
  [4]=>
  object(for_integer_arsort)#4 (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value":"for_integer_arsort":private]=>
    int(-5)
    ["protected_class_value":protected]=>
    int(-4)
  }
}
bool(true)
array(4) {
  ["c"]=>
  object(for_string_arsort)#7 (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value":"for_string_arsort":private]=>
    string(1) "W"
    ["protected_class_value":protected]=>
    string(1) "c"
  }
  ["a"]=>
  object(for_string_arsort)#5 (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value":"for_string_arsort":private]=>
    string(3) "AXX"
    ["protected_class_value":protected]=>
    string(1) "d"
  }
  ["b"]=>
  object(for_string_arsort)#6 (3) {
    ["public_class_value"]=>
    string(1) "T"
    ["private_class_value":"for_string_arsort":private]=>
    string(1) "t"
    ["protected_class_value":protected]=>
    string(1) "q"
  }
  ["d"]=>
  object(for_string_arsort)#8 (3) {
    ["public_class_value"]=>
    string(2) "PY"
    ["private_class_value":"for_string_arsort":private]=>
    string(2) "py"
    ["protected_class_value":protected]=>
    string(1) "s"
  }
}
Done

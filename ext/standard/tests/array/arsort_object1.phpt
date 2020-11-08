--TEST--
Test arsort() function : object functionality - sort objects
--FILE--
<?php
/*
 * testing arsort() by providing integer/string object arrays with following flag values
 * 1. Default flag value
 * 2. SORT_REGULAR - compare items normally
*/

echo "*** Testing arsort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_arsort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }

}

// class declaration for string objects
class for_string_arsort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
   }

  // return string value
  function __tostring() {
   return (string)$this->value;
  }

}

// array of integer objects
$unsorted_int_obj = array (
  1 => new for_integer_arsort(11), 2 =>  new for_integer_arsort(66),
  3 => new for_integer_arsort(23), 4 => new for_integer_arsort(-5),
  5 => new for_integer_arsort(0.001), 6 => new for_integer_arsort(0)
);

// array of string objects
$unsorted_str_obj = array (
  "a" => new for_string_arsort("axx"), "b" => new for_string_arsort("t"),
  "c" => new for_string_arsort("w"), "d" => new for_string_arsort("py"),
  "e" => new for_string_arsort("apple"), "f" => new for_string_arsort("Orange"),
  "g" => new for_string_arsort("Lemon"), "h" => new for_string_arsort("aPPle")
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
array(6) {
  [2]=>
  object(for_integer_arsort)#2 (1) {
    ["class_value"]=>
    int(66)
  }
  [3]=>
  object(for_integer_arsort)#3 (1) {
    ["class_value"]=>
    int(23)
  }
  [1]=>
  object(for_integer_arsort)#1 (1) {
    ["class_value"]=>
    int(11)
  }
  [5]=>
  object(for_integer_arsort)#5 (1) {
    ["class_value"]=>
    float(0.001)
  }
  [6]=>
  object(for_integer_arsort)#6 (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(for_integer_arsort)#4 (1) {
    ["class_value"]=>
    int(-5)
  }
}
bool(true)
array(8) {
  ["c"]=>
  object(for_string_arsort)#9 (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["b"]=>
  object(for_string_arsort)#8 (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["d"]=>
  object(for_string_arsort)#10 (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["a"]=>
  object(for_string_arsort)#7 (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["e"]=>
  object(for_string_arsort)#11 (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["h"]=>
  object(for_string_arsort)#14 (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["f"]=>
  object(for_string_arsort)#12 (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["g"]=>
  object(for_string_arsort)#13 (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}

-- Testing arsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [2]=>
  object(for_integer_arsort)#2 (1) {
    ["class_value"]=>
    int(66)
  }
  [3]=>
  object(for_integer_arsort)#3 (1) {
    ["class_value"]=>
    int(23)
  }
  [1]=>
  object(for_integer_arsort)#1 (1) {
    ["class_value"]=>
    int(11)
  }
  [5]=>
  object(for_integer_arsort)#5 (1) {
    ["class_value"]=>
    float(0.001)
  }
  [6]=>
  object(for_integer_arsort)#6 (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(for_integer_arsort)#4 (1) {
    ["class_value"]=>
    int(-5)
  }
}
bool(true)
array(8) {
  ["c"]=>
  object(for_string_arsort)#9 (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["b"]=>
  object(for_string_arsort)#8 (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["d"]=>
  object(for_string_arsort)#10 (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["a"]=>
  object(for_string_arsort)#7 (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["e"]=>
  object(for_string_arsort)#11 (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["h"]=>
  object(for_string_arsort)#14 (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["f"]=>
  object(for_string_arsort)#12 (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["g"]=>
  object(for_string_arsort)#13 (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}
Done

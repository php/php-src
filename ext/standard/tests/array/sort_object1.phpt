--TEST--
Test sort() function : object functionality - sorting objects, 'sort_flags' as default/SORT_REGULAR
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
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }

}

// class declaration for string objects
class for_string_sort
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
$unsorted_int_obj = array(
  new for_integer_sort(11), new for_integer_sort(66),
  new for_integer_sort(23), new for_integer_sort(-5),
  new for_integer_sort(0.001), new for_integer_sort(0)
);

// array of string objects
$unsorted_str_obj = array (
  new for_string_sort("axx"), new for_string_sort("t"),
  new for_string_sort("w"), new for_string_sort("py"),
  new for_string_sort("apple"), new for_string_sort("Orange"),
  new for_string_sort("Lemon"), new for_string_sort("aPPle")
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
array(6) {
  [0]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [1]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [2]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [3]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [4]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [5]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  [0]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  [1]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  [2]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  [3]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  [4]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  [5]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  [6]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  [7]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}

-- Testing sort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [0]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [1]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [2]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [3]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [4]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [5]=>
  object(for_integer_sort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  [0]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  [1]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  [2]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  [3]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  [4]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  [5]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  [6]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  [7]=>
  object(for_string_sort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}
Done

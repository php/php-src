--TEST--
Test asort() function : object functionality - sort objects  
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
 * 2. SORT_REGULAR - compare items normally
*/

echo "*** Testing asort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_asort
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }

}

// class declaration for string objects
class for_string_asort
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
  1 => new for_integer_asort(11), 2 =>  new for_integer_asort(66),
  3 => new for_integer_asort(23), 4 => new for_integer_asort(-5),
  5 => new for_integer_asort(0.001), 6 => new for_integer_asort(0)
);

// array of string objects
$unsorted_str_obj = array ( 
  "a" => new for_string_asort("axx"), "b" => new for_string_asort("t"),
  "c" => new for_string_asort("w"), "d" => new for_string_asort("py"),
  "e" => new for_string_asort("apple"), "f" => new for_string_asort("Orange"),
  "g" => new for_string_asort("Lemon"), "h" => new for_string_asort("aPPle")
);


echo "\n-- Testing asort() by supplying various object arrays, 'flag' value is defualt --\n";

// testing asort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(asort($temp_array) );
var_dump($temp_array);

// testing asort() function by supplying string object array, flag value is defualt
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

-- Testing asort() by supplying various object arrays, 'flag' value is defualt --
bool(true)
array(6) {
  [4]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [6]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [5]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [1]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [3]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [2]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  ["g"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  ["f"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["h"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["e"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["a"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["d"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["b"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["c"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}

-- Testing asort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [4]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [6]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [5]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [1]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [3]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [2]=>
  object(for_integer_asort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  ["g"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  ["f"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["h"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["e"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["a"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["d"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["b"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["c"]=>
  object(for_string_asort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}
Done

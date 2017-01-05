--TEST--
Test krsort() function : object functionality - sort objects
--FILE--
<?php
/* Prototype  : bool krsort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key in reverse order, maintaining key to data correlation  
 * Source code: ext/standard/array.c
*/
/*
 * testing krsort() by providing array of integer/string objects with following flag values:
 *  1.Defualt flag value
 *  2.SORT_REGULAR - compare items normally
*/

echo "*** Testing krsort() : object functionality ***\n";

// class declaration for integer objects
class IntegerObject
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }
}

// class declaration for string objects
class StringObject
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

// array of integer objects with different key values
$unsorted_int_obj = array ( 
  10 => new IntegerObject(11), 20 =>  new IntegerObject(66),
  3 => new IntegerObject(23), 4 => new IntegerObject(-5),
  50 => new IntegerObject(0.001), 6 => new IntegerObject(0)
);

// array of string objects with different key values
$unsorted_str_obj = array ( 
  "axx" => new StringObject("axx"), "t" => new StringObject("t"),
  "w" => new StringObject("w"), "py" => new StringObject("py"),
  "apple" => new StringObject("apple"), "Orange" => new StringObject("Orange"),
  "Lemon" => new StringObject("Lemon"), "aPPle" => new StringObject("aPPle")
);


echo "\n-- Testing krsort() by supplying various object arrays, 'flag' value is defualt --\n";

// testing krsort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(krsort($temp_array) );
var_dump($temp_array);

// testing krsort() function by supplying string object array, flag value is defualt
$temp_array = $unsorted_str_obj;
var_dump(krsort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing krsort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(krsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing krsort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(krsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing krsort() : object functionality ***

-- Testing krsort() by supplying various object arrays, 'flag' value is defualt --
bool(true)
array(6) {
  [50]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [20]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [10]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [6]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [3]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  ["w"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["t"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["py"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["axx"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["apple"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["aPPle"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["Orange"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["Lemon"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}

-- Testing krsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [50]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [20]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [10]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [6]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [3]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  ["w"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["t"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["py"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["axx"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["apple"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["aPPle"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["Orange"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["Lemon"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}
Done

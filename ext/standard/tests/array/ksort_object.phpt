--TEST--
Test ksort() function : object functionality - sort objects
--FILE--
<?php
/* Prototype  : bool ksort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation.  
 * Source code: ext/standard/array.c
*/
/*
 * testing ksort() by providing array ofinteger/string objects with following flag values: 
 *  1.SORT_NUMERIC - compare items numerically
 *  2.SORT_STRING - compare items as strings 
*/

echo "*** Testing ksort() : object functionality ***\n";

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

// array of integer objects
$unsorted_int_obj = array ( 
  11 => new IntegerObject(11), 66 =>  new IntegerObject(66),
  23 => new IntegerObject(23), -5 => new IntegerObject(-5),
  1 => new IntegerObject(0.001), 0 => new IntegerObject(0)
);

// array of string objects
$unsorted_str_obj = array ( 
  "axx" => new StringObject("axx"), "t" => new StringObject("t"),
  "w" => new StringObject("w"), "py" => new StringObject("py"),
  "apple" => new StringObject("apple"), "Orange" => new StringObject("Orange"),
  "Lemon" => new StringObject("Lemon"), "aPPle" => new StringObject("aPPle")
);
echo "\n-- Testing ksort() by supplying various object arrays, 'flag' value is defualt --\n";

// testing ksort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(ksort($temp_array) );
var_dump($temp_array);

// testing ksort() function by supplying string object array, flag value is defualt
$temp_array = $unsorted_str_obj;
var_dump(ksort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying various object arrays, 'flag' value is SORT_REGULAR --\n";
// testing ksort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(ksort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing ksort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(ksort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing ksort() : object functionality ***

-- Testing ksort() by supplying various object arrays, 'flag' value is defualt --
bool(true)
array(6) {
  [-5]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [0]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [1]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [11]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [23]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [66]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  ["Lemon"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  ["Orange"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["aPPle"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["apple"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["axx"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["py"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["t"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["w"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}

-- Testing ksort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [-5]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [0]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [1]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [11]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [23]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [66]=>
  object(IntegerObject)#%d (1) {
    ["class_value"]=>
    int(66)
  }
}
bool(true)
array(8) {
  ["Lemon"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
  ["Orange"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["aPPle"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["apple"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["axx"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["py"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["t"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["w"]=>
  object(StringObject)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
}
Done

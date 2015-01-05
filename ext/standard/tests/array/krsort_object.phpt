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
class Integer
{
  public $class_value;
  // initializing object member value
  function __construct($value){
    $this->class_value = $value;
  }
}

// class declaration for string objects
class String
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
  10 => new Integer(11), 20 =>  new Integer(66),
  3 => new Integer(23), 4 => new Integer(-5),
  50 => new Integer(0.001), 6 => new Integer(0)
);

// array of string objects with different key values
$unsorted_str_obj = array ( 
  "axx" => new String("axx"), "t" => new String("t"),
  "w" => new String("w"), "py" => new String("py"),
  "apple" => new String("apple"), "Orange" => new String("Orange"),
  "Lemon" => new String("Lemon"), "aPPle" => new String("aPPle")
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
  object(Integer)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [20]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [10]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [6]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [3]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  ["w"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["t"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["py"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["axx"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["apple"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["aPPle"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["Orange"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["Lemon"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}

-- Testing krsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [50]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [20]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [10]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [6]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [4]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
  [3]=>
  object(Integer)#%d (1) {
    ["class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  ["w"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  ["t"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  ["py"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  ["axx"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  ["apple"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  ["aPPle"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  ["Orange"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  ["Lemon"]=>
  object(String)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}
Done

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
 *  1.default flag value
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


echo "\n-- Testing krsort() by supplying various object arrays, 'flag' value is default --\n";

// testing krsort() function by supplying integer object array, flag value is default
$temp_array = $unsorted_int_obj;
var_dump(krsort($temp_array) );
var_dump($temp_array);

// testing krsort() function by supplying string object array, flag value is default
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

-- Testing krsort() by supplying various object arrays, 'flag' value is default --
bool(true)
array(6) {
  [50]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    float(0.001)
  }
  [20]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(66)
  }
  [10]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(11)
  }
  [6]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(0)
  }
  [4]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(-5)
  }
  [3]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  [u"w"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(1) "w"
  }
  [u"t"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(1) "t"
  }
  [u"py"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(2) "py"
  }
  [u"axx"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(3) "axx"
  }
  [u"apple"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "apple"
  }
  [u"aPPle"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "aPPle"
  }
  [u"Orange"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(6) "Orange"
  }
  [u"Lemon"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "Lemon"
  }
}

-- Testing krsort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(6) {
  [50]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    float(0.001)
  }
  [20]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(66)
  }
  [10]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(11)
  }
  [6]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(0)
  }
  [4]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(-5)
  }
  [3]=>
  object(Integer)#%d (1) {
    [u"class_value"]=>
    int(23)
  }
}
bool(true)
array(8) {
  [u"w"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(1) "w"
  }
  [u"t"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(1) "t"
  }
  [u"py"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(2) "py"
  }
  [u"axx"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(3) "axx"
  }
  [u"apple"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "apple"
  }
  [u"aPPle"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "aPPle"
  }
  [u"Orange"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(6) "Orange"
  }
  [u"Lemon"]=>
  object(String)#%d (1) {
    [u"class_value"]=>
    unicode(5) "Lemon"
  }
}
Done

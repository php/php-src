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
 * 1. default flag value
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
    [u"public_class_value"]=>
    int(-88)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(-5)
    [u"protected_class_value":protected]=>
    int(-4)
  }
  [1]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(11)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(33)
    [u"protected_class_value":protected]=>
    int(2)
  }
  [3]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(23)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(32)
    [u"protected_class_value":protected]=>
    int(6)
  }
  [2]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(44)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(66)
    [u"protected_class_value":protected]=>
    int(3)
  }
}
bool(true)
array(4) {
  [u"d"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(2) "PY"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(2) "py"
    [u"protected_class_value":protected]=>
    unicode(1) "s"
  }
  [u"b"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "T"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(1) "t"
    [u"protected_class_value":protected]=>
    unicode(1) "q"
  }
  [u"a"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(3) "axx"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(3) "AXX"
    [u"protected_class_value":protected]=>
    unicode(1) "d"
  }
  [u"c"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "w"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(1) "W"
    [u"protected_class_value":protected]=>
    unicode(1) "c"
  }
}

-- Testing asort() by supplying various object arrays, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [4]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(-88)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(-5)
    [u"protected_class_value":protected]=>
    int(-4)
  }
  [1]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(11)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(33)
    [u"protected_class_value":protected]=>
    int(2)
  }
  [3]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(23)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(32)
    [u"protected_class_value":protected]=>
    int(6)
  }
  [2]=>
  object(for_integer_asort)#%d (3) {
    [u"public_class_value"]=>
    int(44)
    [u"private_class_value":u"for_integer_asort":private]=>
    int(66)
    [u"protected_class_value":protected]=>
    int(3)
  }
}
bool(true)
array(4) {
  [u"d"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(2) "PY"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(2) "py"
    [u"protected_class_value":protected]=>
    unicode(1) "s"
  }
  [u"b"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "T"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(1) "t"
    [u"protected_class_value":protected]=>
    unicode(1) "q"
  }
  [u"a"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(3) "axx"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(3) "AXX"
    [u"protected_class_value":protected]=>
    unicode(1) "d"
  }
  [u"c"]=>
  object(for_string_asort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "w"
    [u"private_class_value":u"for_string_asort":private]=>
    unicode(1) "W"
    [u"protected_class_value":protected]=>
    unicode(1) "c"
  }
}
Done

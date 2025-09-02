--TEST--
Test rsort() function : object functionality - different visibilities
--FILE--
<?php
/*
 * Test functionality of rsort() with objects where properties have different visibilities
 */

echo "*** Testing rsort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_rsort
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
class for_string_rsort
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

$unsorted_int_obj = array(
  new for_integer_rsort(11,33,30),
  new for_integer_rsort(66,44,4),
  new for_integer_rsort(-88,-5,5),
  new for_integer_rsort(0.001,99.5,0.1)
);

// array of string objects
$unsorted_str_obj = array (
  new for_string_rsort("axx","AXX","ass"),
  new for_string_rsort("t","eee","abb"),
  new for_string_rsort("w","W", "c"),
  new for_string_rsort("py","PY", "pt"),
);


echo "\n-- Sort flag = default --\n";

// testing rsort() function by supplying integer object array, flag value is default
$temp_array = $unsorted_int_obj;
var_dump(rsort($temp_array) );
var_dump($temp_array);

// testing rsort() function by supplying string object array, flag value is default
$temp_array = $unsorted_str_obj;
var_dump(rsort($temp_array) );
var_dump($temp_array);

echo "\n-- Sort flag = SORT_REGULAR --\n";
// testing rsort() function by supplying integer object array, flag value = SORT_REGULAR
$temp_array = $unsorted_int_obj;
var_dump(rsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

// testing rsort() function by supplying string object array, flag value = SORT_REGULAR
$temp_array = $unsorted_str_obj;
var_dump(rsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "Done";
?>
--EXPECTF--
*** Testing rsort() : object functionality ***

-- Sort flag = default --
bool(true)
array(4) {
  [0]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(66)
    ["private_class_value":"for_integer_rsort":private]=>
    int(44)
    ["protected_class_value":protected]=>
    int(4)
  }
  [1]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value":"for_integer_rsort":private]=>
    int(33)
    ["protected_class_value":protected]=>
    int(30)
  }
  [2]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    float(0.001)
    ["private_class_value":"for_integer_rsort":private]=>
    float(99.5)
    ["protected_class_value":protected]=>
    float(0.1)
  }
  [3]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value":"for_integer_rsort":private]=>
    int(-5)
    ["protected_class_value":protected]=>
    int(5)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value":"for_string_rsort":private]=>
    string(1) "W"
    ["protected_class_value":protected]=>
    string(1) "c"
  }
  [1]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(1) "t"
    ["private_class_value":"for_string_rsort":private]=>
    string(3) "eee"
    ["protected_class_value":protected]=>
    string(3) "abb"
  }
  [2]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(2) "py"
    ["private_class_value":"for_string_rsort":private]=>
    string(2) "PY"
    ["protected_class_value":protected]=>
    string(2) "pt"
  }
  [3]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value":"for_string_rsort":private]=>
    string(3) "AXX"
    ["protected_class_value":protected]=>
    string(3) "ass"
  }
}

-- Sort flag = SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(66)
    ["private_class_value":"for_integer_rsort":private]=>
    int(44)
    ["protected_class_value":protected]=>
    int(4)
  }
  [1]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(11)
    ["private_class_value":"for_integer_rsort":private]=>
    int(33)
    ["protected_class_value":protected]=>
    int(30)
  }
  [2]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    float(0.001)
    ["private_class_value":"for_integer_rsort":private]=>
    float(99.5)
    ["protected_class_value":protected]=>
    float(0.1)
  }
  [3]=>
  object(for_integer_rsort)#%d (3) {
    ["public_class_value"]=>
    int(-88)
    ["private_class_value":"for_integer_rsort":private]=>
    int(-5)
    ["protected_class_value":protected]=>
    int(5)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(1) "w"
    ["private_class_value":"for_string_rsort":private]=>
    string(1) "W"
    ["protected_class_value":protected]=>
    string(1) "c"
  }
  [1]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(1) "t"
    ["private_class_value":"for_string_rsort":private]=>
    string(3) "eee"
    ["protected_class_value":protected]=>
    string(3) "abb"
  }
  [2]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(2) "py"
    ["private_class_value":"for_string_rsort":private]=>
    string(2) "PY"
    ["protected_class_value":protected]=>
    string(2) "pt"
  }
  [3]=>
  object(for_string_rsort)#%d (3) {
    ["public_class_value"]=>
    string(3) "axx"
    ["private_class_value":"for_string_rsort":private]=>
    string(3) "AXX"
    ["protected_class_value":protected]=>
    string(3) "ass"
  }
}
Done

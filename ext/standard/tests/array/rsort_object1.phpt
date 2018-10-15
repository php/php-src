--TEST--
Test rsort() function : object functionality
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of rsort() with objects
 */

echo "*** Testing rsort() : object functionality ***\n";

// class declaration for integer objects
class for_integer_rsort
{
	public $class_value;
	// initializing object member value
	function __construct($value){
		$this->class_value = $value;
	}

}

// class declaration for string objects
class for_string_rsort
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
  new for_integer_rsort(11), new for_integer_rsort(66),
  new for_integer_rsort(23), new for_integer_rsort(-5),
  new for_integer_rsort(0.001), new for_integer_rsort(0)
);

// array of string objects
$unsorted_str_obj = array (
  new for_string_rsort("axx"), new for_string_rsort("t"),
  new for_string_rsort("w"), new for_string_rsort("py"),
  new for_string_rsort("apple"), new for_string_rsort("Orange"),
  new for_string_rsort("Lemon"), new for_string_rsort("aPPle")
);


echo "\n-- Sort flag = default --\n";

// testing rsort() function by supplying integer object array, flag value is defualt
$temp_array = $unsorted_int_obj;
var_dump(rsort($temp_array) );
var_dump($temp_array);

// testing rsort() function by supplying string object array, flag value is defualt
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
array(6) {
  [0]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [1]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [2]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [3]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [4]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [5]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
}
bool(true)
array(8) {
  [0]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  [1]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  [2]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  [3]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  [4]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  [5]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  [6]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  [7]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}

-- Sort flag = SORT_REGULAR --
bool(true)
array(6) {
  [0]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(66)
  }
  [1]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(23)
  }
  [2]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(11)
  }
  [3]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    float(0.001)
  }
  [4]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(0)
  }
  [5]=>
  object(for_integer_rsort)#%d (1) {
    ["class_value"]=>
    int(-5)
  }
}
bool(true)
array(8) {
  [0]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(1) "w"
  }
  [1]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(1) "t"
  }
  [2]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(2) "py"
  }
  [3]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(3) "axx"
  }
  [4]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "apple"
  }
  [5]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "aPPle"
  }
  [6]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(6) "Orange"
  }
  [7]=>
  object(for_string_rsort)#%d (1) {
    ["class_value"]=>
    string(5) "Lemon"
  }
}
Done

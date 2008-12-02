--TEST--
Test rsort() function : object functionality - different visibilities
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order 
 * Source code: ext/standard/array.c
 */

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
	function __tounicode() {
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
array(4) {
  [0]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(66)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(44)
    [u"protected_class_value":protected]=>
    int(4)
  }
  [1]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(11)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(33)
    [u"protected_class_value":protected]=>
    int(30)
  }
  [2]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    float(0.001)
    [u"private_class_value":u"for_integer_rsort":private]=>
    float(99.5)
    [u"protected_class_value":protected]=>
    float(0.1)
  }
  [3]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(-88)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(-5)
    [u"protected_class_value":protected]=>
    int(5)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "w"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(1) "W"
    [u"protected_class_value":protected]=>
    unicode(1) "c"
  }
  [1]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "t"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(3) "eee"
    [u"protected_class_value":protected]=>
    unicode(3) "abb"
  }
  [2]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(2) "py"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(2) "PY"
    [u"protected_class_value":protected]=>
    unicode(2) "pt"
  }
  [3]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(3) "axx"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(3) "AXX"
    [u"protected_class_value":protected]=>
    unicode(3) "ass"
  }
}

-- Sort flag = SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(66)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(44)
    [u"protected_class_value":protected]=>
    int(4)
  }
  [1]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(11)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(33)
    [u"protected_class_value":protected]=>
    int(30)
  }
  [2]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    float(0.001)
    [u"private_class_value":u"for_integer_rsort":private]=>
    float(99.5)
    [u"protected_class_value":protected]=>
    float(0.1)
  }
  [3]=>
  object(for_integer_rsort)#%d (3) {
    [u"public_class_value"]=>
    int(-88)
    [u"private_class_value":u"for_integer_rsort":private]=>
    int(-5)
    [u"protected_class_value":protected]=>
    int(5)
  }
}
bool(true)
array(4) {
  [0]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "w"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(1) "W"
    [u"protected_class_value":protected]=>
    unicode(1) "c"
  }
  [1]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(1) "t"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(3) "eee"
    [u"protected_class_value":protected]=>
    unicode(3) "abb"
  }
  [2]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(2) "py"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(2) "PY"
    [u"protected_class_value":protected]=>
    unicode(2) "pt"
  }
  [3]=>
  object(for_string_rsort)#%d (3) {
    [u"public_class_value"]=>
    unicode(3) "axx"
    [u"private_class_value":u"for_string_rsort":private]=>
    unicode(3) "AXX"
    [u"protected_class_value":protected]=>
    unicode(3) "ass"
  }
}
Done
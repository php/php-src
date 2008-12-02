--TEST--
Test usort() function : object functionality - different number of properties
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function 
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array of objects which have a different number of properties
 * to test behaviour of usort()
 */

echo "*** Testing usort() : object functionality ***\n";

function simple_cmp($value1, $value2)
{
	if($value1 == $value2) {
		return 0;
	}
	else if($value1 > $value2) {
		return 1;
	}
	else
	return -1;
}

// comparison function for SimpleClass2 objects which has more than one member
function multiple_cmp($value1, $value2)
{
	if($value1->getValue() == $value2->getValue())
	return 0;
	else if($value1->getValue() > $value2->getValue())
	return 1;
	else
	return -1;
}

// Simple class with single property
class SimpleClass1
{
	private $int_value;

	public function __construct($value) {
		$this->int_value = $value;
	}
}

// Simple class with more than one property
class SimpleClass2
{
	private $int_value;
	protected $float_value;
	public $string_value;
	public function __construct($int, $float, $str) {
		$this->int_value = $int;
		$this->float_value = $float;
		$this->string_value = $str;
	}
	public function getValue() {
		return $this->int_value;
	}
}

// array of SimpleClass objects with only one property
$array_arg = array(
0 => new SimpleClass1(10),
1 => new SimpleClass1(1),
2 => new SimpleClass1(100),
3 => new SimpleClass1(50)
);
var_dump( usort($array_arg, 'simple_cmp') );
var_dump($array_arg);

// array of SimpleClass objects having more than one properties
$array_arg = array(
0 => new SimpleClass2(2, 3.4, "mango"),
1 => new SimpleClass2(10, 1.2, "apple"),
2 => new SimpleClass2(5, 2.5, "orange"),
);
var_dump( usort($array_arg, 'multiple_cmp') );
var_dump($array_arg);
?>
===DONE===
--EXPECTF--
*** Testing usort() : object functionality ***
bool(true)
array(4) {
  [0]=>
  object(SimpleClass1)#%d (1) {
    ["int_value:private"]=>
    int(1)
  }
  [1]=>
  object(SimpleClass1)#%d (1) {
    ["int_value:private"]=>
    int(10)
  }
  [2]=>
  object(SimpleClass1)#%d (1) {
    ["int_value:private"]=>
    int(50)
  }
  [3]=>
  object(SimpleClass1)#%d (1) {
    ["int_value:private"]=>
    int(100)
  }
}
bool(true)
array(3) {
  [0]=>
  object(SimpleClass2)#%d (3) {
    ["int_value:private"]=>
    int(2)
    ["float_value:protected"]=>
    float(3.4)
    ["string_value"]=>
    string(5) "mango"
  }
  [1]=>
  object(SimpleClass2)#%d (3) {
    ["int_value:private"]=>
    int(5)
    ["float_value:protected"]=>
    float(2.5)
    ["string_value"]=>
    string(6) "orange"
  }
  [2]=>
  object(SimpleClass2)#%d (3) {
    ["int_value:private"]=>
    int(10)
    ["float_value:protected"]=>
    float(1.2)
    ["string_value"]=>
    string(5) "apple"
  }
}
===DONE===

--TEST--
Test uasort() function : object functionality
--FILE--
<?php
/*
 * Testing uasort() function with the array of objects
 * array of objects which has only one member variable & more than one member variables
 */

echo "*** Testing uasort() : object functionality ***\n";

// comparison function
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

// comparison function for SimpleClass2 objects which has more than one members
function multiple_cmp($value1, $value2)
{
  if($value1->getValue() == $value2->getValue())
    return 0;
  else if($value1->getValue() > $value2->getValue())
    return 1;
  else
    return -1;
}

// Simple class with single member variable
class SimpleClass1
{
  private $int_value;

  public function __construct($value) {
    $this->int_value = $value;
  }
}

// Simple class with more than one member variables
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

// array of SimpleClass objects with only one member
$array_arg = array(
  0 => new SimpleClass1(10),
  1 => new SimpleClass1(1),
  2 => new SimpleClass1(100),
  3 => new SimpleClass1(50)
);
var_dump( uasort($array_arg, 'simple_cmp') );
var_dump($array_arg);

// array of SimpleClass objects having more than one members
$array_arg = array(
  0 => new SimpleClass2(2, 3.4, "mango"),
  1 => new SimpleClass2(10, 1.2, "apple"),
  2 => new SimpleClass2(5, 2.5, "orange"),
);
var_dump( uasort($array_arg, 'multiple_cmp') );
var_dump($array_arg);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : object functionality ***
bool(true)
array(4) {
  [1]=>
  object(SimpleClass1)#2 (1) {
    ["int_value":"SimpleClass1":private]=>
    int(1)
  }
  [0]=>
  object(SimpleClass1)#1 (1) {
    ["int_value":"SimpleClass1":private]=>
    int(10)
  }
  [3]=>
  object(SimpleClass1)#4 (1) {
    ["int_value":"SimpleClass1":private]=>
    int(50)
  }
  [2]=>
  object(SimpleClass1)#3 (1) {
    ["int_value":"SimpleClass1":private]=>
    int(100)
  }
}
bool(true)
array(3) {
  [0]=>
  object(SimpleClass2)#5 (3) {
    ["int_value":"SimpleClass2":private]=>
    int(2)
    ["float_value":protected]=>
    float(3.4)
    ["string_value"]=>
    string(5) "mango"
  }
  [2]=>
  object(SimpleClass2)#7 (3) {
    ["int_value":"SimpleClass2":private]=>
    int(5)
    ["float_value":protected]=>
    float(2.5)
    ["string_value"]=>
    string(6) "orange"
  }
  [1]=>
  object(SimpleClass2)#6 (3) {
    ["int_value":"SimpleClass2":private]=>
    int(10)
    ["float_value":protected]=>
    float(1.2)
    ["string_value"]=>
    string(5) "apple"
  }
}
Done

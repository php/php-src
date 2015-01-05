--TEST--
Test usort() function : object functionality - Different types of classes
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function 
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array of objects which are either: 
 * 1. Empty
 * 2. Static
 * 2. Inherited
 * to test behaviour of usort()
 */

echo "*** Testing usort() : object functionality ***\n";

function cmp_function($value1, $value2)
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

// Class without any member
class EmptyClass
{
}

// Class with static member
class StaticClass
{
  public static $static_value;
  public function __construct($value) {
    StaticClass::$static_value = $value;
  }
}

// Abstract class
abstract class AbstractClass
{
  public $pub_value;
  public abstract function abstractMethod();
}

// Child class extending abstract class
class ChildClass extends AbstractClass
{
  public $child_value = 100;
  public function abstractMethod() {
    $pub_value = 5;
  }
  public function __construct($value) {
    $this->child_value = $value;
  }
}

// Testing uasort with StaticClass objects as elements of 'array_arg'
echo "-- Testing usort() with StaticClass objects --\n";
$array_arg = array(
  0 => new StaticClass(20),
  1 => new StaticClass(50),
  2 => new StaticClass(15),
  3 => new StaticClass(70),
);
var_dump( usort($array_arg, 'cmp_function') );
var_dump($array_arg);

// Testing uasort with EmptyClass objects as elements of 'array_arg'
echo "-- Testing usort() with EmptyClass objects --\n";
$array_arg = array(
  0 => new EmptyClass(),
  1 => new EmptyClass(),
  2 => new EmptyClass(),
  3 => new EmptyClass(),
);
var_dump( usort($array_arg, 'cmp_function') );
var_dump($array_arg);

// Testing uasort with ChildClass objects as elements of 'array_arg'
echo "-- Testing usort() with ChildClass objects --\n";
$array_arg = array(
  0 => new ChildClass(20),
  1 => new ChildClass(500),
  2 => new ChildClass(15),
  3 => new ChildClass(700),
);
var_dump( usort($array_arg, 'cmp_function') );
var_dump($array_arg);
?>
===DONE===
--EXPECTF--
*** Testing usort() : object functionality ***
-- Testing usort() with StaticClass objects --
bool(true)
array(4) {
  [0]=>
  object(StaticClass)#%d (0) {
  }
  [1]=>
  object(StaticClass)#%d (0) {
  }
  [2]=>
  object(StaticClass)#%d (0) {
  }
  [3]=>
  object(StaticClass)#%d (0) {
  }
}
-- Testing usort() with EmptyClass objects --
bool(true)
array(4) {
  [0]=>
  object(EmptyClass)#%d (0) {
  }
  [1]=>
  object(EmptyClass)#%d (0) {
  }
  [2]=>
  object(EmptyClass)#%d (0) {
  }
  [3]=>
  object(EmptyClass)#%d (0) {
  }
}
-- Testing usort() with ChildClass objects --
bool(true)
array(4) {
  [0]=>
  object(ChildClass)#%d (2) {
    ["child_value"]=>
    int(15)
    ["pub_value"]=>
    NULL
  }
  [1]=>
  object(ChildClass)#%d (2) {
    ["child_value"]=>
    int(20)
    ["pub_value"]=>
    NULL
  }
  [2]=>
  object(ChildClass)#%d (2) {
    ["child_value"]=>
    int(500)
    ["pub_value"]=>
    NULL
  }
  [3]=>
  object(ChildClass)#%d (2) {
    ["child_value"]=>
    int(700)
    ["pub_value"]=>
    NULL
  }
}
===DONE===
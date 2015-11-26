--TEST--
Test array_map() function : usage variations - object functionality
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Testing array_map() for object functionalities:
 *   1) simple class with variable and method
 *   2) class without members
 *   3) class with only one method and no variable
 *   4) abstract and child class
 *   5) class with static and final members
 *   6) interface and implemented class
 */
echo "*** Testing array_map() : object functionality ***\n";

echo "-- simple class with public variable and method --\n";
class SimpleClass
{
  public $var1 = 1;
  public function square($n) {
    return $n * $n;
  }
}
function test($cb, $args) {
  echo join('::', $cb) . "\n";
  var_dump(array_map($cb, $args));
}
test(array('SimpleClass', 'square'), array(1, 2));

echo "\n-- simple class with private variable and method --\n";
class SimpleClassPri
{
  private $var1 = 10;
  private function add($n) {
    return $var + $n;
  }
}
test(array('SimpleClassPri', 'add'), array(1));

echo "\n-- simple class with protected variable and method --\n";
class SimpleClassPro
{
  protected $var1 = 5;
  protected function mul($n) {
    return $var1 * $n;
  }
}
test(array('SimpleClassPro', 'mul'), array(2));

echo "\n-- class without members --\n";
class EmptyClass
{
}
test(array('EmptyClass'), array(1, 2));

echo "\n-- abstract class --\n";
abstract class AbstractClass
{
  protected $var2 = 5;
  abstract function emptyFunction();
}

// class deriving the above abstract class
class ChildClass extends AbstractClass
{
  private $var3;
  public function emptyFunction() {
    echo "defined in child\n";
  }
}
test(array('ChildClass', 'emptyFunction'), array(1, 2));

echo "\n-- class with final method --\n";
class FinalClass
{
  private $var4;
  final function finalMethod() {
    echo "This function can't be overloaded\n";
  }
}
test(array('FinalClass', 'finalMethod'), array(1, 2));

echo "\n-- class with static members --\n";
class StaticClass
{
  static $var5 = 2;
  public static function square($n) {
    return ($n * $n);
  }
  private static function cube($n) {
    return ($n * $n * $n);
  }
  protected static function retVal($n)  {
    return array($n);
  }
}
test(array('StaticClass', 'square'), array(1, 2));
test(array('StaticClass', 'cube'), array(2));
test(array('StaticClass', 'retVal'), array(3, 4));

echo "-- class implementing an interface --\n";
interface myInterface
{
  public function toImplement();
}
class InterClass implements myInterface
{
  public static function square($n) {
    return ($n * $n);
  }
  public function toImplement() {
    return 1;
  }
}
test(array('InterClass', 'square'), array(1, 2));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
*** Testing array_map() : object functionality ***
-- simple class with public variable and method --
SimpleClass::square

Deprecated: array_map() expects parameter 1 to be a valid callback, non-static method SimpleClass::square() should not be called statically in %sarray_map_object1.php on line %d
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}

-- simple class with private variable and method --
SimpleClassPri::add

Warning: array_map() expects parameter 1 to be a valid callback, cannot access private method SimpleClassPri::add() in %sarray_map_object1.php on line %d
NULL

-- simple class with protected variable and method --
SimpleClassPro::mul

Warning: array_map() expects parameter 1 to be a valid callback, cannot access protected method SimpleClassPro::mul() in %sarray_map_object1.php on line %d
NULL

-- class without members --
EmptyClass

Warning: array_map() expects parameter 1 to be a valid callback, array must have exactly two members in %sarray_map_object1.php on line %d
NULL

-- abstract class --
ChildClass::emptyFunction

Deprecated: array_map() expects parameter 1 to be a valid callback, non-static method ChildClass::emptyFunction() should not be called statically in %sarray_map_object1.php on line %d
defined in child
defined in child
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

-- class with final method --
FinalClass::finalMethod

Deprecated: array_map() expects parameter 1 to be a valid callback, non-static method FinalClass::finalMethod() should not be called statically in %sarray_map_object1.php on line %d
This function can't be overloaded
This function can't be overloaded
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}

-- class with static members --
StaticClass::square
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}
StaticClass::cube

Warning: array_map() expects parameter 1 to be a valid callback, cannot access private method StaticClass::cube() in %sarray_map_object1.php on line %d
NULL
StaticClass::retVal

Warning: array_map() expects parameter 1 to be a valid callback, cannot access protected method StaticClass::retVal() in %sarray_map_object1.php on line %d
NULL
-- class implementing an interface --
InterClass::square
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}
===DONE===

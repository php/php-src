--TEST--
Test array_map() function : usage variations - object functionality
--FILE--
<?php
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
  public static function square($n) {
    return $n * $n;
  }
}
function test($cb, $args) {
  echo join('::', $cb) . "\n";
  try {
    var_dump(array_map($cb, $args));
  } catch (TypeError $e) {
    echo $e->getMessage(), "\n";
  }
}
test(array('SimpleClass', 'square'), array(1, 2));

echo "\n-- simple class with private variable and method --\n";
class SimpleClassPri
{
  private $var1 = 10;
  private static function add($n) {
    return $var + $n;
  }
}
test(array('SimpleClassPri', 'add'), array(1));

echo "\n-- simple class with protected variable and method --\n";
class SimpleClassPro
{
  protected $var1 = 5;
  protected static function mul($n) {
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
  abstract static function emptyFunction();
}

// class deriving the above abstract class
class ChildClass extends AbstractClass
{
  private $var3;
  public static function emptyFunction() {
    echo "defined in child\n";
  }
}
test(array('ChildClass', 'emptyFunction'), array(1, 2));

echo "\n-- class with final method --\n";
class FinalClass
{
  private $var4;
  final static function finalMethod() {
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
--EXPECT--
*** Testing array_map() : object functionality ***
-- simple class with public variable and method --
SimpleClass::square
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}

-- simple class with private variable and method --
SimpleClassPri::add
array_map(): Argument #1 ($callback) must be a valid callback or null, cannot access private method SimpleClassPri::add()

-- simple class with protected variable and method --
SimpleClassPro::mul
array_map(): Argument #1 ($callback) must be a valid callback or null, cannot access protected method SimpleClassPro::mul()

-- class without members --
EmptyClass
array_map(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members

-- abstract class --
ChildClass::emptyFunction
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
array_map(): Argument #1 ($callback) must be a valid callback or null, cannot access private method StaticClass::cube()
StaticClass::retVal
array_map(): Argument #1 ($callback) must be a valid callback or null, cannot access protected method StaticClass::retVal()
-- class implementing an interface --
InterClass::square
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}

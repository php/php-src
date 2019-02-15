--TEST--
Test array_filter() function : object functionality
--FILE--
<?php
/* Prototype  : array array_filter(array $input [, callback $callback])
 * Description: Filters elements from the array via the callback.
 * Source code: ext/standard/array.c
*/

/* This file uses 'input' array with different types of objects and passes
 * it to array_filter() to test object functionality
 * i.e. object of simple class with members and functions
 * object of empty class
 * object of child class extending abstract class
 * object of class containing static member
 */

echo "*** Testing array_filter() : object functionality ***\n";

// simple class with members - variable and method
class SimpleClass
{
  public $var1 = 10;
  public function check() {
    return $var1;
  }
}

// class without members
class EmptyClass
{
}

// abstract class
abstract class AbstractClass
{
  protected $var2 = 5;
  abstract function emptyMethod();
}

// class deriving above abstract class
class ChildClass extends AbstractClass
{
  private $var3;
  public function emptyMethod() {
    echo "defined in child";
  }
}

// class with final method
class FinalClass
{
  private $var4;
  final function finalMethod() {
    echo 'This can not be overloaded';
  }
}

// class with static members
class StaticClass
{
  static $var5 = 5;
  public static function staticMethod() {
    echo 'this is static method';
  }
}

// Callback function which returns always true
function always_true($input)
{
  return true;
}

// Callback function which returns always false
function always_false($input)
{
  return false;
}

// 'input' array containing objects as elements
$input = array(
  new SimpleClass(),
  new EmptyClass(),
  new ChildClass(),
  new FinalClass(),
  new StaticClass()
);


// with default callback
var_dump( array_filter($input) );

// with always_true callback function
var_dump( array_filter($input, "always_true") );

// with always_false callback function
var_dump( array_filter($input, "always_false") );

echo "Done"
?>
--EXPECTF--
*** Testing array_filter() : object functionality ***
array(5) {
  [0]=>
  object(SimpleClass)#%d (1) {
    ["var1"]=>
    int(10)
  }
  [1]=>
  object(EmptyClass)#%d (0) {
  }
  [2]=>
  object(ChildClass)#%d (2) {
    ["var3":"ChildClass":private]=>
    NULL
    ["var2":protected]=>
    int(5)
  }
  [3]=>
  object(FinalClass)#%d (1) {
    ["var4":"FinalClass":private]=>
    NULL
  }
  [4]=>
  object(StaticClass)#%d (0) {
  }
}
array(5) {
  [0]=>
  object(SimpleClass)#%d (1) {
    ["var1"]=>
    int(10)
  }
  [1]=>
  object(EmptyClass)#%d (0) {
  }
  [2]=>
  object(ChildClass)#%d (2) {
    ["var3":"ChildClass":private]=>
    NULL
    ["var2":protected]=>
    int(5)
  }
  [3]=>
  object(FinalClass)#%d (1) {
    ["var4":"FinalClass":private]=>
    NULL
  }
  [4]=>
  object(StaticClass)#%d (0) {
  }
}
array(0) {
}
Done

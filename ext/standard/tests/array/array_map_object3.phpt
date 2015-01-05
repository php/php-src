--TEST--
Test array_map() function : object functionality - class methods as callback function
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Testing array_map() for object functionality with following callback function variations:
 *   1) child class method using parent object
 *   2) parent class method using child object
 *   3) child class method using parent class
 *   4) parent class method using child class
 */
echo "*** Testing array_map() : class methods as callback function ***\n";

$arr1 = array(1, 5, 7);

class ParentClass
{
  public $var1 = 10;
  public static function staticParent1($n) {
    return $n;
  }
  private static function staticParent2($n) {
    return $n;
  }
}

class ChildClass extends ParentClass 
{
  var $parent_obj;
  public function __construct ( ) {
    $this->parent_obj = new ParentClass();
  }
  public $var2 = 5;
  public static function staticChild($n) {
    return $n;
  }
  public function nonstaticChild($n) {
    return $n;
  }
}

$childobj = new ChildClass();
$parentobj = new ParentClass();

echo "-- accessing parent method from child class --\n";
var_dump( array_map(array('ChildClass', 'staticParent1'), $arr1) );

echo "-- accessing child method from parent class --\n";
var_dump( array_map(array('ParentClass', 'staticChild'), $arr1) );

echo "-- accessing parent method using child class object --\n";
var_dump( array_map(array($childobj, 'staticParent1'), $arr1) );

echo "-- accessing child method using parent class object --\n";
var_dump( array_map(array($parentobj, 'staticChild'), $arr1) );

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : class methods as callback function ***
-- accessing parent method from child class --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(7)
}
-- accessing child method from parent class --

Warning: array_map() expects parameter 1 to be a valid callback, class 'ParentClass' does not have a method 'staticChild' in %s on line %d
NULL
-- accessing parent method using child class object --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(7)
}
-- accessing child method using parent class object --

Warning: array_map() expects parameter 1 to be a valid callback, class 'ParentClass' does not have a method 'staticChild' in %s on line %d
NULL
Done

--TEST--
Test count() function : object functionality - object with Countable interface
--SKIPIF--
<?php 
// Skip the test case if Standard PHP Library(spl) is not installed
  if( !extension_loaded('spl'))
  {
     die('skip spl is not installed');
  }
?>
--FILE--
<?php
/* Prototype  : int count($mixed var[, int $mode])
 * Description: Counts an elements in an array. If Standard PHP library is installed, 
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 */

echo "*** Testing count() : object functionality ***\n";

echo "-- Testing count() with an object which implements Countable interface --\n";
class count_class implements Countable 
{
  public $member1;
  private $member2;
  protected $member3;
 
  public function count()
  {
    return 3; // return the count of member variables in the object
  }
}

$obj = new count_class();

echo "-- Testing count() in default mode --\n";
var_dump( count($obj) );
echo "-- Testing count() in COUNT_NORMAL mode --\n";
var_dump( count($obj, COUNT_NORMAL) );
echo "-- Testing count() in COUNT_RECURSIVE mode --\n";
var_dump( count($obj, COUNT_RECURSIVE) );

echo "Done";
?>
--EXPECTF--
*** Testing count() : object functionality ***
-- Testing count() with an object which implements Countable interface --
-- Testing count() in default mode --
int(3)
-- Testing count() in COUNT_NORMAL mode --
int(3)
-- Testing count() in COUNT_RECURSIVE mode --
int(3)
Done

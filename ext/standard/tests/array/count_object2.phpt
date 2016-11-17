--TEST--
Test count() function : object functionality - objects without Countable interface
--FILE--
<?php
/* Prototype  : int count($mixed var[, int $mode] )
 * Description: Counts an elements in an array. If Standard PHP library is installed,
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 */

echo "*** Testing count() : object functionality ***\n";

echo "--- Testing count() with objects which doesn't implement Countable interface ---\n";

// class without member
class test
{ 
  // no members
}

// class with only members and with out member functions 
class test1
{
  public $member1;
  var $var1;
  private $member2;
  protected $member3;

  // no member functions
}

// class with only member functions
class test2
{
  // no data members 

  public function display()
  {
    echo " Class Name : test2\n";
  }
}

// child class which inherits parent test2
class child_test2 extends test2
{
  public $child_member1;
  private $child_member2;
}

// abstract class
abstract class abstract_class
{
  public $member1;
  private $member2;
 
  abstract protected function display();
}

// implement abstract 'abstract_class' class
class concrete_class extends abstract_class
{
  protected function display()
  {
    echo " class name is : concrete_class \n ";
  }
}

$objects = array (
  /* 1  */  new test(),
            new test1(),
            new test2(),
            new child_test2(),
  /* 5  */  new concrete_class()
);

$counter = 1;
for($i = 0; $i < count($objects); $i++)
{
  echo "-- Iteration $counter --\n";
  $var = $objects[$i];

  echo "Default Mode: ";
  var_dump( count($var) );
  echo "\n";
  
  echo "COUNT_NORMAL Mode: ";
  var_dump( count($var, COUNT_NORMAL) );
  echo "\n";

  echo "COUNT_RECURSIVE Mode: ";
  var_dump( count($var, COUNT_RECURSIVE) );
  echo "\n";
 
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing count() : object functionality ***
--- Testing count() with objects which doesn't implement Countable interface ---
-- Iteration 1 --
Default Mode: int(1)

COUNT_NORMAL Mode: int(1)

COUNT_RECURSIVE Mode: int(1)

-- Iteration 2 --
Default Mode: int(1)

COUNT_NORMAL Mode: int(1)

COUNT_RECURSIVE Mode: int(1)

-- Iteration 3 --
Default Mode: int(1)

COUNT_NORMAL Mode: int(1)

COUNT_RECURSIVE Mode: int(1)

-- Iteration 4 --
Default Mode: int(1)

COUNT_NORMAL Mode: int(1)

COUNT_RECURSIVE Mode: int(1)

-- Iteration 5 --
Default Mode: int(1)

COUNT_NORMAL Mode: int(1)

COUNT_RECURSIVE Mode: int(1)

Done

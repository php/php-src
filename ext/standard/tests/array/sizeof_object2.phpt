--TEST--
Test sizeof() function : object functionality - objects without Countable interface
--FILE--
<?php
/* Prototype  : int sizeof($mixed var[, int $mode] )
 * Description: Counts an elements in an array. If Standard PHP library is installed,
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: count()
 */

echo "*** Testing sizeof() : object functionality ***\n";

echo "--- Testing sizeof() with objects which doesn't implement Countable interface ---\n";

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
  var_dump( sizeof($var) );
  echo "\n";

  echo "COUNT_NORMAL Mode: ";
  var_dump( sizeof($var, COUNT_NORMAL) );
  echo "\n";

  echo "COUNT_RECURSIVE Mode: ";
  var_dump( sizeof($var, COUNT_RECURSIVE) );
  echo "\n";

  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing sizeof() : object functionality ***
--- Testing sizeof() with objects which doesn't implement Countable interface ---
-- Iteration 1 --
Default Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_NORMAL Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_RECURSIVE Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

-- Iteration 2 --
Default Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_NORMAL Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_RECURSIVE Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

-- Iteration 3 --
Default Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_NORMAL Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_RECURSIVE Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

-- Iteration 4 --
Default Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_NORMAL Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_RECURSIVE Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

-- Iteration 5 --
Default Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_NORMAL Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

COUNT_RECURSIVE Mode: 
Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

Done

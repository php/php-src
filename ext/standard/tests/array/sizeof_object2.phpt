--TEST--
Test sizeof() function : object functionality - objects without Countable interface
--FILE--
<?php
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
  try {
    var_dump( sizeof($var) );
  } catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
  }

  echo "COUNT_NORMAL Mode: ";
  try {
    var_dump( sizeof($var, COUNT_NORMAL) );
  } catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
  }

  echo "COUNT_RECURSIVE Mode: ";
  try {
    var_dump( sizeof($var, COUNT_RECURSIVE) );
  } catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
  }

  $counter++;
}

echo "Done";
?>
--EXPECT--
*** Testing sizeof() : object functionality ***
--- Testing sizeof() with objects which doesn't implement Countable interface ---
-- Iteration 1 --
Default Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test given
COUNT_NORMAL Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test given
COUNT_RECURSIVE Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test given
-- Iteration 2 --
Default Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test1 given
COUNT_NORMAL Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test1 given
COUNT_RECURSIVE Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test1 given
-- Iteration 3 --
Default Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test2 given
COUNT_NORMAL Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test2 given
COUNT_RECURSIVE Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, test2 given
-- Iteration 4 --
Default Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, child_test2 given
COUNT_NORMAL Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, child_test2 given
COUNT_RECURSIVE Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, child_test2 given
-- Iteration 5 --
Default Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, concrete_class given
COUNT_NORMAL Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, concrete_class given
COUNT_RECURSIVE Mode: sizeof(): Argument #1 ($value) must be of type Countable|array, concrete_class given
Done

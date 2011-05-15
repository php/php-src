--TEST--
Test array_fill() function : usage variations - various object values for 'val' argument
--SKIPIF--
<?php if (version_compare(zend_version(), '2.4.0', '<')) die('skip ZendEngine 2.4 needed'); ?>
--FILE--
<?php
/* Prototype  : array array_fill(int $start_key, int $num, mixed $val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val 
 * Source code: ext/standard/array.c
 */

/*
 * testing array_fill() by passing various  object values for 'val' argument
 */

echo "*** Testing array_fill() : usage variations ***\n";

// Initialise function arguments not being substituted 
$start_key = 0;
$num = 2;

// class without a member
class Test
{
}

//class with public member, static member , constant and consturctor to initialize the public member 
class Test1
{
  const test1_constant = "test1";
  public static $test1_static = 0;
  public $member1;
  var $var1 = 30;
  var $var2;

  function __construct($value1 , $value2)
  {
    $this->member1 = $value1;
    $this->var2 = $value2;
  }
}

// child class which inherits parent class test1
class Child_test1 extends Test1
{
  public $member2;

  function __construct($value1 , $value2 , $value3)
  {
    parent::__construct($value1 , $value2);
    $this->member2 = $value3;
  }
}

//class with private member, static member, constant and constructor to initialize the private member
class Test2
{
  const test2_constant = "test2";
  public static $test2_static = 0;
  private $member1;
  var $var1 = 30;
  var $var2;

  function __construct($value1 , $value2)
  {
    $this->member1 = $value1;
    $this->var2 = $value2;
  }
}

// child class which inherits parent class test2
class Child_test2 extends Test2
{
  private $member1;

  function __construct($value1 , $value2 , $value3)
  {
    parent::__construct($value1 , $value2);
    $this->member1 = $value3;
  }
}

// class with protected member, static member, constant and consturctor to initialize the protected member 
class Test3
{
  const test3_constant = "test3";
  public static $test3_static = 0;
  protected $member1;
  var $var1 = 30;
  var $var2; 

  function __construct($value1 , $value2)
  {
     $this->member1 = $value1;
     $this->var2 = $value2;
  }
}

// child class which inherits parent class test3
class Child_test3 extends Test3
{
  protected $member1;

  function __construct($value1 , $value2 , $value3)
  {
    parent::__construct($value1 , $value2);
    $this->member1 = $value3;
  }
}

// class with public, private, protected members, static, constant members and constructor to initialize all the members
class Test4
{
  const test4_constant = "test4";
  public static $test4_static = 0;
  public $member1;
  private $member2;
  protected $member3;

  function __construct($value1 , $value2 , $value3)
  {
    $this->member1 = $value1;
    $this->member2 = $value2;
    $this->member3 = $value3;
  }
}

// child class which inherits parent class test4
class Child_test4 extends Test4
{
  var $var1;
  
  function __construct($value1 , $value2 , $value3 , $value4)
  {
    parent::__construct($value1 , $value2 , $value3);
    $this->var1 = $value4;
  }
}

// abstract class with public, private, protected members 
abstract class AbstractClass
{
  public $member1;
  private $member2;
  protected $member3;
  var $var1 = 30;
  
  abstract protected function display();
}

// implement abstract 'AbstractClass' class
class ConcreteClass1 extends AbstractClass
{
  protected function display()
  {
    echo "class name is ConcreteClass1 \n";
  }
}


// declarationn of the interface 'iTemplate'
interface iTemplate
{
  public function display();
}

// implement the interface 'iTemplate'
class Template1 implements iTemplate
{
  public function display()
  {
    echo "class name is Template1\n";
  }
}

//array of object values for 'val' argument
$objects = array(
  
  /* 1  */  new Test(),
            new Test1(100 , 101),
            new Child_test1(100 , 101 , 102),
            new Test2(100 , 101),
  /* 5  */  new Child_test2(100 , 101 , 102),
            new Test3(100 , 101),
            new Child_test3(100 , 101 , 102),
            new Test4( 100 , 101 , 102),
            new Child_test4(100 , 101 , 102 , 103),
            new ConcreteClass1(),
  /* 11 */  new Template1()
);

// loop through each element of the array for 'val' argument 
// check the working of array_fill()
echo "--- Testing array_fill() with different object values for 'val' argument ---\n";
$counter = 1;
for($index = 0; $index < count($objects); $index ++)
{
  echo "-- Iteration $counter --\n";
  $val = $objects[$index];

  var_dump( array_fill($start_key,$num,$val) );

  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_fill() : usage variations ***
--- Testing array_fill() with different object values for 'val' argument ---
-- Iteration 1 --
array(2) {
  [0]=>
  object(Test)#%d (0) {
  }
  [1]=>
  object(Test)#%d (0) {
  }
}
-- Iteration 2 --
array(2) {
  [0]=>
  object(Test1)#%d (3) {
    ["member1"]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
  [1]=>
  object(Test1)#%d (3) {
    ["member1"]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
}
-- Iteration 3 --
array(2) {
  [0]=>
  object(Child_test1)#%d (4) {
    ["member2"]=>
    int(102)
    ["member1"]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
  [1]=>
  object(Child_test1)#%d (4) {
    ["member2"]=>
    int(102)
    ["member1"]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
}
-- Iteration 4 --
array(2) {
  [0]=>
  object(Test2)#%d (3) {
    ["member1":"Test2":private]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
  [1]=>
  object(Test2)#%d (3) {
    ["member1":"Test2":private]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
}
-- Iteration 5 --
array(2) {
  [0]=>
  object(Child_test2)#%d (4) {
    ["member1":"Child_test2":private]=>
    int(102)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
    ["member1":"Test2":private]=>
    int(100)
  }
  [1]=>
  object(Child_test2)#%d (4) {
    ["member1":"Child_test2":private]=>
    int(102)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
    ["member1":"Test2":private]=>
    int(100)
  }
}
-- Iteration 6 --
array(2) {
  [0]=>
  object(Test3)#%d (3) {
    ["member1":protected]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
  [1]=>
  object(Test3)#%d (3) {
    ["member1":protected]=>
    int(100)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
}
-- Iteration 7 --
array(2) {
  [0]=>
  object(Child_test3)#%d (3) {
    ["member1":protected]=>
    int(102)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
  [1]=>
  object(Child_test3)#%d (3) {
    ["member1":protected]=>
    int(102)
    ["var1"]=>
    int(30)
    ["var2"]=>
    int(101)
  }
}
-- Iteration 8 --
array(2) {
  [0]=>
  object(Test4)#%d (3) {
    ["member1"]=>
    int(100)
    ["member2":"Test4":private]=>
    int(101)
    ["member3":protected]=>
    int(102)
  }
  [1]=>
  object(Test4)#%d (3) {
    ["member1"]=>
    int(100)
    ["member2":"Test4":private]=>
    int(101)
    ["member3":protected]=>
    int(102)
  }
}
-- Iteration 9 --
array(2) {
  [0]=>
  object(Child_test4)#%d (4) {
    ["var1"]=>
    int(103)
    ["member1"]=>
    int(100)
    ["member2":"Test4":private]=>
    int(101)
    ["member3":protected]=>
    int(102)
  }
  [1]=>
  object(Child_test4)#%d (4) {
    ["var1"]=>
    int(103)
    ["member1"]=>
    int(100)
    ["member2":"Test4":private]=>
    int(101)
    ["member3":protected]=>
    int(102)
  }
}
-- Iteration 10 --
array(2) {
  [0]=>
  object(ConcreteClass1)#%d (4) {
    ["member1"]=>
    NULL
    ["member2":"AbstractClass":private]=>
    NULL
    ["member3":protected]=>
    NULL
    ["var1"]=>
    int(30)
  }
  [1]=>
  object(ConcreteClass1)#%d (4) {
    ["member1"]=>
    NULL
    ["member2":"AbstractClass":private]=>
    NULL
    ["member3":protected]=>
    NULL
    ["var1"]=>
    int(30)
  }
}
-- Iteration 11 --
array(2) {
  [0]=>
  object(Template1)#%d (0) {
  }
  [1]=>
  object(Template1)#%d (0) {
  }
}
Done

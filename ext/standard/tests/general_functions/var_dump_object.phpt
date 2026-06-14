--TEST--
Test var_dump() function
--FILE--
<?php

function check_var_dump($variables) {
    $counter = 1;
    foreach ($variables as $variable) {
        echo "\n-- Iteration $counter --\n";
        var_dump($variable);
        $counter++;
    }
}

echo "\n*** Testing var_dump() on object variables ***\n";
#[AllowDynamicProperties]
class object_class
{
    var       $value;
    public    $public_var1 = 10;
    private   $private_var1 = 20;
    private   $private_var2;
    protected $protected_var1 = "string_1";
    protected $protected_var2;

    function __construct() {
        $this->value = 50;
        $this->public_var2 = 11;
        $this->private_var2 = 21;
        $this->protected_var2 = "string_2";
    }

    public function foo1() {
        echo "foo1() is called\n";
    }
    protected function foo2() {
        echo "foo2() is called\n";
    }
    private function foo3() {
        echo "foo3() is called\n";
    }
}

class no_member_class {}

/* class with member as object of other class */
#[AllowDynamicProperties]
class contains_object_class
{
   var       $p = 30;
   var       $class_object1;
   public    $class_object2;
   private   $class_object3;
   protected $class_object4;
   var       $no_member_class_object;

   public function func() {
     echo "func() is called \n";
   }

   function __construct() {
     $this->class_object1 = new object_class();
     $this->class_object2 = new object_class();
     $this->class_object3 = $this->class_object1;
     $this->class_object4 = $this->class_object2;
     $this->no_member_class_object = new no_member_class();
     $this->class_object5 = $this;   //recursive reference
   }
}

$objects = [
    new object_class,
    new no_member_class,
    new contains_object_class,
];
check_var_dump($objects);

?>
--EXPECT--
*** Testing var_dump() on object variables ***

-- Iteration 1 --
object(object_class)#1 (7) {
  ["value"]=>
  int(50)
  ["public_var1"]=>
  int(10)
  ["private_var1":"object_class":private]=>
  int(20)
  ["private_var2":"object_class":private]=>
  int(21)
  ["protected_var1":protected]=>
  string(8) "string_1"
  ["protected_var2":protected]=>
  string(8) "string_2"
  ["public_var2"]=>
  int(11)
}

-- Iteration 2 --
object(no_member_class)#2 (0) {
}

-- Iteration 3 --
object(contains_object_class)#3 (7) {
  ["p"]=>
  int(30)
  ["class_object1"]=>
  object(object_class)#4 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object2"]=>
  object(object_class)#5 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object3":"contains_object_class":private]=>
  object(object_class)#4 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["class_object4":protected]=>
  object(object_class)#5 (7) {
    ["value"]=>
    int(50)
    ["public_var1"]=>
    int(10)
    ["private_var1":"object_class":private]=>
    int(20)
    ["private_var2":"object_class":private]=>
    int(21)
    ["protected_var1":protected]=>
    string(8) "string_1"
    ["protected_var2":protected]=>
    string(8) "string_2"
    ["public_var2"]=>
    int(11)
  }
  ["no_member_class_object"]=>
  object(no_member_class)#6 (0) {
  }
  ["class_object5"]=>
  *RECURSION*
}

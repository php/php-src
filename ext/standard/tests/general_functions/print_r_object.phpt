--TEST--
Test print_r() function
--FILE--
<?php

function check_printr($variables) {
    $counter = 1;
    foreach ($variables as $variable) {
        echo "\n-- Iteration $counter --\n";
        print_r($variable, false);
        // $return=TRUE, print_r() will return its output, instead of printing it
        $ret_string = print_r($variable, true); //$ret_string captures the output
        echo "\n$ret_string\n";
        $counter++;
    }
}

echo "\n*** Testing print_r() on object variables ***\n";
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
check_printr($objects);

?>
--EXPECT--
*** Testing print_r() on object variables ***

-- Iteration 1 --
object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)

object_class Object
(
    [value] => 50
    [public_var1] => 10
    [private_var1:object_class:private] => 20
    [private_var2:object_class:private] => 21
    [protected_var1:protected] => string_1
    [protected_var2:protected] => string_2
    [public_var2] => 11
)


-- Iteration 2 --
no_member_class Object
(
)

no_member_class Object
(
)


-- Iteration 3 --
contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

contains_object_class Object
(
    [p] => 30
    [class_object1] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object2] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object3:contains_object_class:private] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [class_object4:protected] => object_class Object
        (
            [value] => 50
            [public_var1] => 10
            [private_var1:object_class:private] => 20
            [private_var2:object_class:private] => 21
            [protected_var1:protected] => string_1
            [protected_var2:protected] => string_2
            [public_var2] => 11
        )

    [no_member_class_object] => no_member_class Object
        (
        )

    [class_object5] => contains_object_class Object
 *RECURSION*
)

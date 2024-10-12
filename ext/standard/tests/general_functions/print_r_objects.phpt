--TEST--
print_r(): Test printing of objects
--INI--
precision=14
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        ob_start();
        $should_be_true = print_r($variable, /* $return */ false);
        $output_content = ob_get_flush();
        var_dump($should_be_true);
        $print_r_with_return = print_r($variable, /* $return */ true);
        var_dump($output_content === $print_r_with_return);
        $output_content = null;
        $counter++;
    }
}

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

/* class with no member */
class no_member_class {
    // no members
}

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
    }
}

/* objects of different classes */
$obj = new contains_object_class;
$temp_class_obj = new object_class();

$objects = [
    new object_class,
    new no_member_class,
    new contains_object_class,
    $obj,
    $obj->class_object1,
    $obj->class_object2,
    $obj->no_member_class_object,
    $temp_class_obj,
];

check_print_r($objects);

?>
--EXPECT--
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
bool(true)
bool(true)
-- Iteration 2 --
no_member_class Object
(
)
bool(true)
bool(true)
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

)
bool(true)
bool(true)
-- Iteration 4 --
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

)
bool(true)
bool(true)
-- Iteration 5 --
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
bool(true)
bool(true)
-- Iteration 6 --
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
bool(true)
bool(true)
-- Iteration 7 --
no_member_class Object
(
)
bool(true)
bool(true)
-- Iteration 8 --
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
bool(true)
bool(true)

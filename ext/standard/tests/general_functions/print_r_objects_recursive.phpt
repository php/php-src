--TEST--
print_r(): Test objects with circular reference
--FILE--
<?php

class object_class {
    public $var;
}

$recursion_obj1 = new object_class();
$recursion_obj2 = new object_class();
$recursion_obj1->var = &$recursion_obj2;
$recursion_obj2->var = &$recursion_obj1;
print_r($recursion_obj2);

?>
--EXPECT--
object_class Object
(
    [var] => object_class Object
        (
            [var] => object_class Object
 *RECURSION*
        )

)

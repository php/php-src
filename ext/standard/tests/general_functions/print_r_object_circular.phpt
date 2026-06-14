--TEST--
Test print_r() function function with circular objects
--FILE--
<?php

class Circular {
    public object $obj;
}

$recursion_obj1 = new Circular();
$recursion_obj2 = new Circular();
$recursion_obj1->obj = $recursion_obj2;
$recursion_obj2->obj = $recursion_obj1;
print_r($recursion_obj2);

?>
--EXPECT--
Circular Object
(
    [obj] => Circular Object
        (
            [obj] => Circular Object
 *RECURSION*
        )

)

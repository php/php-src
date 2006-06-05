--TEST--
Bug #30332 (zend.ze1_compatibility_mode isnt fully compatable with array_push())
--INI--
zend.ze1_compatibility_mode=on
error_reporting=4095
--FILE--
<?php
class x { };

$first = new x;
$second = $first;
$container = array();
array_push($container, $first);

$first->first = " im in the first";

print_r($first);
print_r($second);
print_r($container);
?>
--EXPECTF--
Strict Standards: Implicit cloning object of class 'x' because of 'zend.ze1_compatibility_mode' in %sbug30332.php on line 4

Strict Standards: Implicit cloning object of class 'x' because of 'zend.ze1_compatibility_mode' in %sbug30332.php on line 5

Strict Standards: Implicit cloning object of class 'x' because of 'zend.ze1_compatibility_mode' in %sbug30332.php on line 7
x Object
(
    [first] =>  im in the first
)
x Object
(
)
Array
(
    [0] => x Object
        (
        )

)

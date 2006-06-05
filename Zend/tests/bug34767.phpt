--TEST--
Bug #34767 (Zend Engine 1 Compatibility not copying objects correctly)
--INI--
zend.ze1_compatibility_mode=1
error_reporting=4095
--FILE--
<?php
$a->y = &new stdClass();
print_r($a);
$b = $a;
$a->y->z = 1;
print_r($b);
?>
--EXPECTF--

Strict Standards: Assigning the return value of new by reference is deprecated in %sbug34767.php on line 2
stdClass Object
(
    [y] => stdClass Object
        (
        )

)

Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug34767.php on line 4
stdClass Object
(
    [y] => stdClass Object
        (
            [z] => 1
        )

)

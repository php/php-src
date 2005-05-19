--TEST--
Bug #31828 (Crash with zend.ze1_compatibility_mode=On)
--INI--
zend.ze1_compatibility_mode=on
error_reporting=4095
--FILE--
<?php
$o = new stdClass();
$o->id = 77;
$o->name = "Aerospace";
$a[] = $o;
$a = $a[0];
print_r($a);
?>
--EXPECTF--
Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug31828.php on line 2

Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug31828.php on line 5

Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug31828.php on line 6
stdClass Object
(
    [id] => 77
    [name] => Aerospace
)

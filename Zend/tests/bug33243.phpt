--TEST--
Bug #33243 (ze1_compatibility_mode does not work as expected)
--INI--
zend.ze1_compatibility_mode=1
error_reporting=4095
--FILE--
<?php
$a->y->z = 0;
$b = $a;      // should perform deep copy of $a
$b->y->z = 1; // hence this should have no effect on $a
var_dump($a);
?>
--EXPECTF--
Strict Standards: Creating default object from empty value in %sbug33243.php on line 2

Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug33243.php on line 3

Strict Standards: Implicit cloning object of class 'stdClass' because of 'zend.ze1_compatibility_mode' in %sbug33243.php on line 5
object(stdClass)#%d (1) {
  ["y"]=>
  object(stdClass)#%d (1) {
    ["z"]=>
    int(0)
  }
}

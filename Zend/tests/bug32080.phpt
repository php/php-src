--TEST--
Bug #32080 (segfault when assigning object to itself with zend.ze1_compatibility_mode=On)
--INI--
zend.ze1_compatibility_mode=on
error_reporting=4095
--FILE--
<?php
class test { }
$t = new test;
$t = $t; // gives segfault
var_dump($t);
?>
--EXPECTF--
Strict Standards: Implicit cloning object of class 'test' because of 'zend.ze1_compatibility_mode' in %sbug32080.php on line 3

Strict Standards: Implicit cloning object of class 'test' because of 'zend.ze1_compatibility_mode' in %sbug32080.php on line 5
object(test)#%d (0) {
}

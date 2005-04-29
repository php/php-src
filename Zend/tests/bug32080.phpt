--TSTE--
Bug #32080 (segfault when assigning object to itself with zend.ze1_compatibility_mode=On)
--INI--
zend.ze1_compatibility_mode=on
--FILE--
<?php
class test { }
$t = new test;
$t = $t; // gives segfault
var_dump($t);
?>
--EXPECT--
object(test)#2 (0) {
}

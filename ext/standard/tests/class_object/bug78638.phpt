--TEST--
FR: #78638 (__PHP_Incomplete_Class should be final)
--FILE--
<?php
$c = new class('bar') extends __PHP_Incomplete_Class {
};
?>
--EXPECTF--
Fatal error: Class class@anonymous may not inherit from final class (__PHP_Incomplete_Class) in %sbug78638.php on line %d

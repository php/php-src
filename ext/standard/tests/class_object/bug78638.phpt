--TEST--
FR: #78638 (__PHP_Incomplete_Class should be final)
--FILE--
<?php
$c = new class('bar') extends __PHP_Incomplete_Class {
};
?>
--EXPECTF--
Fatal error: Class __PHP_Incomplete_Class@anonymous cannot extend final class __PHP_Incomplete_Class in %s on line %d

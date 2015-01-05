--TEST--
Bug #63882 (zend_std_compare_objects crash on recursion)
--FILE--
<?php
class Test { public $x = 5; }

$testobj1 = new Test;
$testobj2 = new Test;
$testobj1->x = $testobj1;
$testobj2->x = $testobj2;

var_dump($testobj1 == $testobj2);
?>
--EXPECTF--
Fatal error: Nesting level too deep - recursive dependency? in %sbug63882.php on line 9

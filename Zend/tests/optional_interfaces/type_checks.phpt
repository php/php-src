--TEST--
Only the existing interfaces pass the type checks
--INI--
zend.exception_ignore_args = On
--FILE--
<?php

interface ExistingInterface {}

class TestClass implements ?ExistingInterface, ?NonExistantInterface {}

function f1(ExistingInterface $x) { echo "F1"; }
function f2(NonExistantInterface $x) { echo "F2"; }

$c = new TestClass;

f1($c);
f2($c)

?>
--EXPECTF--
F1
Fatal error: Uncaught TypeError: f2(): Argument #1 ($x) must be of type NonExistantInterface, TestClass given, called in %stype_checks.php on line %d and defined in %stype_checks.php:%d
Stack trace:
#0 %stype_checks.php(%d): f2(Object(TestClass))
#1 {main}
  thrown in %stype_checks.php on line %d

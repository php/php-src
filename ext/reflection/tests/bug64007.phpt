--TEST--
Bug #64007 (There is an ability to create instance of Generator by hand)
--FILE--
<?php
$reflection = new ReflectionClass('Generator');
try {
    $generator = $reflection->newInstanceWithoutConstructor();
    var_dump($generator);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

$generator  = $reflection->newInstance();
var_dump($generator);
?>
--EXPECTF--
string(%d) "Class Generator is an internal class marked as final that cannot be instantiated without invoking its constructor"

Fatal error: Uncaught ReflectionException: Access to non-public constructor of class Generator, the "Generator" class is reserved for internal use and cannot be manually instantiated, in %sbug64007.php:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstance()
#1 {main}
  thrown in %sbug64007.php on line %d

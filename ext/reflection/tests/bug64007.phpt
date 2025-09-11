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

$generator = $reflection->newInstance();
var_dump($generator);
?>
--EXPECTF--
string(47) "Class Generator cannot be instantiated manually"

Fatal error: Uncaught ReflectionException: Class Generator cannot be instantiated manually in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstance()
#1 {main}
  thrown in %s on line %d

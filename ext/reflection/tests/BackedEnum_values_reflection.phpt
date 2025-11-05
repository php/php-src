--TEST--
BackedEnum::values() appears in reflection
--FILE--
<?php
enum Status: string {
    case Active = 'active';
}
$method = new ReflectionMethod(Status::class, 'values');
var_dump($method->isStatic());
var_dump($method->isPublic());
var_dump($method->getNumberOfParameters());
?>
--EXPECT--
bool(true)
bool(true)
int(0)


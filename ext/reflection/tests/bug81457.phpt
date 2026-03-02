--TEST--
Bug #81457: Enum ReflectionMethod->getDeclaringClass() return a ReflectionClass
--FILE--
<?php

enum testEnum {
    case A;
    case B;
    
    public function foo () {}
}

$re = new ReflectionEnum(testEnum::class);
$me = $re->getMethod('foo');

echo $me->getDeclaringClass()::class, "\n";

$rc = new ReflectionClass(testEnum::class);
$mc = $re->getMethod('foo');

echo $mc->getDeclaringClass()::class, "\n";

?>
--EXPECT--
ReflectionEnum
ReflectionEnum

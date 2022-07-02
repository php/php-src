--TEST--
ReflectionClass::isUserDefined()
--FILE--
<?php
class C {
}

$r1 = new ReflectionClass("stdClass");
$r2 = new ReflectionClass("ReflectionClass");
$r3 = new ReflectionClass("ReflectionProperty");
$r4 = new ReflectionClass("Exception");
$r5 = new ReflectionClass("C");

var_dump($r1->isUserDefined(), $r2->isUserDefined(), $r3->isUserDefined(),
         $r4->isUserDefined(), $r5->isUserDefined());
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

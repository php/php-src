--TEST--
ReflectionClass::isInternal()
--FILE--
<?php
class C {
}

$r1 = new ReflectionClass("stdClass");
$r2 = new ReflectionClass("ReflectionClass");
$r3 = new ReflectionClass("ReflectionProperty");
$r4 = new ReflectionClass("Exception");
$r5 = new ReflectionClass("C");

var_dump($r1->isInternal(), $r2->isInternal(), $r3->isInternal(),
		 $r4->isInternal(), $r5->isInternal());
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

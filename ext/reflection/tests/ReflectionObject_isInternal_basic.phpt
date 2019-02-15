--TEST--
ReflectionObject::isInternal() - basic function test
--FILE--
<?php
class C {
}

$r1 = new ReflectionObject(new stdClass);
$r2 = new ReflectionObject(new ReflectionClass('C'));
$r3 = new ReflectionObject(new ReflectionProperty('Exception', 'message'));
$r4 = new ReflectionObject(new Exception);
$r5 = new ReflectionObject(new C);

var_dump($r1->isInternal(), $r2->isInternal(), $r3->isInternal(),
		 $r4->isInternal(), $r5->isInternal());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)

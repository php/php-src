--TEST--
GH-20830: ReflectionProperty::isInitialized() should not trigger lazy object initialization
--FILE--
<?php

class TestClass {
    public readonly int $id;
    public string $name;
}

$rc = new ReflectionClass(TestClass::class);

$obj = $rc->newLazyGhost(function ($ghost) {
    $rp = new ReflectionProperty(TestClass::class, 'id');
    $rp->setRawValueWithoutLazyInitialization($ghost, 123);
    $ghost->name = 'initialized';
    echo 'Ghost initialized' . PHP_EOL;
});

echo 'Created lazy ghost' . PHP_EOL;
var_dump($rc->isUninitializedLazyObject($obj));

echo 'Checking if $id is initialized...' . PHP_EOL;
$rp = new ReflectionProperty(TestClass::class, 'id');
var_dump($rp->isInitialized($obj));
var_dump($rc->isUninitializedLazyObject($obj));

echo 'Accessing $id value...' . PHP_EOL;
$rp->getValue($obj);
var_dump($rp->isInitialized($obj));
var_dump($rc->isUninitializedLazyObject($obj));
?>
--EXPECT--
Created lazy ghost
bool(true)
Checking if $id is initialized...
bool(false)
bool(true)
Accessing $id value...
Ghost initialized
bool(true)
bool(false)

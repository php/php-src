--TEST--
Test ReflectionClass::setAccessible().
--FILE--
<?php

private class A {
    public function myMethod() { print __METHOD__ . "\n"; }
}

class B {
    public function myMethod() { print __METHOD__ . "\n"; }
}

$privateClass = new ReflectionClass('A');
$publicClass  = new ReflectionClass('B');

echo "==BEFORE==\n";

try {
    $privateClass->newInstance();

    echo "Should not work\n";
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $publicClass->newInstance();

    echo "Should work\n";
} catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

echo "==AFTER==\n";

$privateClass->setAccessible(TRUE);
$publicClass->setAccessible(TRUE);

$privateClass->newInstance();
echo "Should work\n";

$publicClass->newInstance();
echo "Should work\n";

?>
--EXPECT--
==BEFORE==
string(51) "Unable to create private class A through reflection"
Should work
==AFTER==
Should work
Should work

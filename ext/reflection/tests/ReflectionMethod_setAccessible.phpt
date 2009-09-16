--TEST--
Test ReflectionMethod::setAccessible().
--FILE--
<?php
class A {
    private function aPrivate($a) { print __METHOD__ . "\n"; }
    private static function aPrivateStatic($a) { print __METHOD__ . "\n"; }
    protected function aProtected($a) { print __METHOD__ . "\n"; }
    protected static function aProtectedStatic($a) { print __METHOD__ . "\n"; }
}

$private         = new ReflectionMethod('A', 'aPrivate');
$privateStatic   = new ReflectionMethod('A', 'aPrivateStatic');
$protected       = new ReflectionMethod('A', 'aProtected');
$protectedStatic = new ReflectionMethod('A', 'aProtectedStatic');

try {
    $private->invoke(new A, NULL);
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $private->invokeArgs(new A, array(NULL));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $privateStatic->invoke(NULL, NULL);
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $privateStatic->invokeArgs(NULL, array(NULL));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $protected->invoke(new A, NULL);
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $protected->invokeArgs(new A, array(NULL));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $protectedStatic->invoke(NULL, NULL);
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    $protectedStatic->invokeArgs(NULL, array(NULL));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

$private->setAccessible(TRUE);
$privateStatic->setAccessible(TRUE);
$protected->setAccessible(TRUE);
$protectedStatic->setAccessible(TRUE);

$private->invoke(new A, NULL);
$private->invokeArgs(new A, array(NULL));
$privateStatic->invoke(NULL, NULL);
$privateStatic->invokeArgs(NULL, array(NULL));
$protected->invoke(new A, NULL);
$protected->invokeArgs(new A, array(NULL));
$protectedStatic->invoke(NULL, NULL);
$protectedStatic->invokeArgs(NULL, array(NULL));
?>
--EXPECT--
string(73) "Trying to invoke private method A::aPrivate() from scope ReflectionMethod"
string(73) "Trying to invoke private method A::aPrivate() from scope ReflectionMethod"
string(79) "Trying to invoke private method A::aPrivateStatic() from scope ReflectionMethod"
string(79) "Trying to invoke private method A::aPrivateStatic() from scope ReflectionMethod"
string(77) "Trying to invoke protected method A::aProtected() from scope ReflectionMethod"
string(77) "Trying to invoke protected method A::aProtected() from scope ReflectionMethod"
string(83) "Trying to invoke protected method A::aProtectedStatic() from scope ReflectionMethod"
string(83) "Trying to invoke protected method A::aProtectedStatic() from scope ReflectionMethod"
A::aPrivate
A::aPrivate
A::aPrivateStatic
A::aPrivateStatic
A::aProtected
A::aProtected
A::aProtectedStatic
A::aProtectedStatic

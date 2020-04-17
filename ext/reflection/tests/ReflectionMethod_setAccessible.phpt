--TEST--
Test that ReflectionMethod::setAccessible() has no effects
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

$private->invoke(new A, NULL);
$private->invokeArgs(new A, array(NULL));
$privateStatic->invoke(NULL, NULL);
$privateStatic->invokeArgs(NULL, array(NULL));
$protected->invoke(new A, NULL);
$protected->invokeArgs(new A, array(NULL));
$protectedStatic->invoke(NULL, NULL);
$protectedStatic->invokeArgs(NULL, array(NULL));

$private->setAccessible(FALSE);
$privateStatic->setAccessible(FALSE);
$protected->setAccessible(FALSE);
$protectedStatic->setAccessible(FALSE);

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
A::aPrivate
A::aPrivate
A::aPrivateStatic
A::aPrivateStatic
A::aProtected
A::aProtected
A::aProtectedStatic
A::aProtectedStatic
A::aPrivate
A::aPrivate
A::aPrivateStatic
A::aPrivateStatic
A::aProtected
A::aProtected
A::aProtectedStatic
A::aProtectedStatic

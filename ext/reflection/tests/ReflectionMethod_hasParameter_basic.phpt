--TEST--
ReflectionMethod::hasParameter()
--FILE--
<?php
class C {
    public function foo(string $bar) {}
}

$method = new ReflectionMethod('WeakReference', 'create');
var_dump($method->hasParameter('object'));
var_dump($method->hasParameter('Object'));
var_dump($method->hasParameter('string'));
var_dump($method->hasParameter(1));
var_dump($method->hasParameter(2));

$method = new ReflectionMethod('C', 'foo');
var_dump($method->hasParameter('bar'));
var_dump($method->hasParameter('Bar'));
var_dump($method->hasParameter('string'));
var_dump($method->hasParameter(1));
var_dump($method->hasParameter(2));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)

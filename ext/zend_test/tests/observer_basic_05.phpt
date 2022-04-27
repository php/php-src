--TEST--
Observer: Basic observability of functions only (no includes)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_functions=1
--FILE--
<?php
function foo()
{
    echo 'Foo' . PHP_EOL;
}

foo();
include __DIR__ . '/observer.inc';
foo();
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_basic_05.php' -->
<!-- init foo() -->
<foo>
Foo
</foo>
<!-- init '%s%eobserver.inc' -->
<!-- init foo_observer_test() -->
<foo_observer_test>
foo_observer_test
</foo_observer_test>
<foo>
Foo
</foo>

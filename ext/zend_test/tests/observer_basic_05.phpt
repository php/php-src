--TEST--
Observer: Basic observability of functions only (no includes)
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
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
<!-- init '%s/observer_basic_05.php' -->
<!-- init foo() -->
<foo>
Foo
</foo>
<!-- init '%s/observer.inc' -->
<!-- init foo_observer_test() -->
<foo_observer_test>
foo_observer_test
</foo_observer_test>
<foo>
Foo
</foo>

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

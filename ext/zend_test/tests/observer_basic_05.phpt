--TEST--
Observer: Basic observability of functions only (no includes)
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
[should observe '%s/observer_basic_05.php'?]
[should observe foo()?]
<foo>
Foo
</foo>
[should observe '%s/observer.inc'?]
[should observe foo_observer_test()?]
<foo_observer_test>
foo_observer_test
</foo_observer_test>
<foo>
Foo
</foo>

--TEST--
Observer: Basic observability of includes only (no functions)
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_includes=1
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
[should observe '%s/observer_basic_04.php'?]
<file '%s/observer_basic_04.php'>
[should observe foo()?]
Foo
[should observe '%s/observer.inc'?]
  <file '%s/observer.inc'>
[should observe foo_observer_test()?]
foo_observer_test
  </file '%s/observer.inc'>
Foo
</file '%s/observer_basic_04.php'>

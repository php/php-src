--TEST--
Observer: Basic observability of includes
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
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
[should observe '%s/observer_basic_03.php'?]
<file '%s/observer_basic_03.php'>
[should observe foo()?]
  <foo>
Foo
  </foo>
[should observe '%s/observer.inc'?]
  <file '%s/observer.inc'>
[should observe foo_observer_test()?]
    <foo_observer_test>
foo_observer_test
    </foo_observer_test>
  </file '%s/observer.inc'>
  <foo>
Foo
  </foo>
</file '%s/observer_basic_03.php'>

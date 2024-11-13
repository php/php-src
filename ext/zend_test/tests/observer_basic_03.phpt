--TEST--
Observer: Basic observability of includes
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
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
<!-- init '%s%eobserver_basic_03.php' -->
<file '%s%eobserver_basic_03.php'>
  <!-- init foo() -->
  <foo>
Foo
  </foo>
  <!-- init '%s%eobserver.inc' -->
  <file '%s%eobserver.inc'>
    <!-- init foo_observer_test() -->
    <foo_observer_test>
foo_observer_test
    </foo_observer_test>
  </file '%s%eobserver.inc'>
  <foo>
Foo
  </foo>
</file '%s%eobserver_basic_03.php'>

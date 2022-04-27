--TEST--
Observer: Basic observability of includes
--EXTENSIONS--
zend_test
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
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

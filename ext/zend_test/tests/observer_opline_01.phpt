--TEST--
Observer: Ensure opline exists on the execute_data
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_opcode=1
--FILE--
<?php
function foo()
{
    echo 'Foo' . PHP_EOL;
}

foo();
include __DIR__ . '/observer.inc';
echo array_sum([1,2,3]) . PHP_EOL;
foo();
?>
--EXPECTF--
<!-- init '%s/observer_opline_%d.php' -->
<!-- opcode: 'ZEND_INIT_FCALL' -->
<file '%s/observer_opline_%d.php'>
  <!-- opcode: 'ZEND_INIT_FCALL' -->
  <!-- init foo() -->
  <!-- opcode: 'ZEND_ECHO' -->
  <foo>
    <!-- opcode: 'ZEND_ECHO' -->
Foo
    <!-- opcode: 'ZEND_RETURN' -->
  </foo>
  <!-- init '%s/observer.inc' -->
  <!-- opcode: 'ZEND_INIT_FCALL' -->
  <file '%s/observer.inc'>
    <!-- opcode: 'ZEND_INIT_FCALL' -->
    <!-- init foo_observer_test() -->
    <!-- opcode: 'ZEND_ECHO' -->
    <foo_observer_test>
      <!-- opcode: 'ZEND_ECHO' -->
foo_observer_test
      <!-- opcode: 'ZEND_RETURN' -->
    </foo_observer_test>
    <!-- opcode: 'ZEND_RETURN' -->
  </file '%s/observer.inc'>
6
  <foo>
    <!-- opcode: 'ZEND_ECHO' -->
Foo
    <!-- opcode: 'ZEND_RETURN' -->
  </foo>
  <!-- opcode: 'ZEND_RETURN' -->
</file '%s/observer_opline_%d.php'>

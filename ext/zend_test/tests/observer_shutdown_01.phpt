--TEST--
Observer: Function calls from a shutdown handler are observable
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
register_shutdown_function(function () {
    echo 'Shutdown: ' . foo() . PHP_EOL;
});

function bar() {
    return 42;
}

function foo() {
    bar();
    return bar();
}

echo 'Done: ' . bar() . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s/observer_shutdown_%d.php' -->
<file '%s/observer_shutdown_%d.php'>
  <!-- init bar() -->
  <bar>
  </bar:42>
Done: 42
</file '%s/observer_shutdown_%d.php'>
<!-- init {closure}() -->
<{closure}>
  <!-- init foo() -->
  <foo>
    <bar>
    </bar:42>
    <bar>
    </bar:42>
  </foo:42>
Shutdown: 42
</{closure}:NULL>

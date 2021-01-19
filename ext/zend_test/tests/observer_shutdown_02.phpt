--TEST--
Observer: Function calls from a __destruct during shutdown are observable
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
class MyClass
{
    public function __destruct()
    {
        echo 'Shutdown: ' . foo() . PHP_EOL;
    }
}

function bar($arg) {
    return $arg;
}

function foo() {
    bar(41);
    return bar(42);
}

$mc = new MyClass();

echo 'Done: ' . bar(40) . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_shutdown_%d.php' -->
<file '%s%eobserver_shutdown_%d.php'>
  <!-- init bar() -->
  <bar>
  </bar:40>
Done: 40
</file '%s%eobserver_shutdown_%d.php'>
<!-- init MyClass::__destruct() -->
<MyClass::__destruct>
  <!-- init foo() -->
  <foo>
    <bar>
    </bar:41>
    <bar>
    </bar:42>
  </foo:42>
Shutdown: 42
</MyClass::__destruct:NULL>

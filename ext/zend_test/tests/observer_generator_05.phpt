--TEST--
Observer: Generator with uncaught exception
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function fooResults() {
    yield 0;
    yield 1;
    throw new RuntimeException('Oops!');
}

function doSomething() {
    $generator = fooResults();
    foreach ($generator as $value) {
        echo $value . PHP_EOL;
    }

    return 'You should not see this';
}

echo doSomething() . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_generator_%d.php' -->
<file '%s%eobserver_generator_%d.php'>
  <!-- init doSomething() -->
  <doSomething>
    <!-- init fooResults() -->
    <fooResults>
    </fooResults:0>
0
    <fooResults>
    </fooResults:1>
1
    <fooResults>
      <!-- Exception: RuntimeException -->
    </fooResults:NULL>
    <!-- Exception: RuntimeException -->
  </doSomething:NULL>
  <!-- Exception: RuntimeException -->
</file '%s%eobserver_generator_%d.php'>

Fatal error: Uncaught RuntimeException: Oops! in %s%eobserver_generator_%d.php:%d
Stack trace:
#0 %s%eobserver_generator_%d.php(%d): fooResults()
#1 %s%eobserver_generator_%d.php(%d): doSomething()
#2 {main}
  thrown in %s%eobserver_generator_%d.php on line %d

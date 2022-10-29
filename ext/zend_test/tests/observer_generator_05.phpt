--TEST--
Observer: Generator with uncaught exception
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
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
<!-- init '%s' -->
<file '%s'>
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
      <!-- init Exception::__construct() -->
      <Exception::__construct>
      </Exception::__construct:NULL>
      <!-- Exception: RuntimeException -->
    </fooResults:NULL>
    <!-- Exception: RuntimeException -->
  </doSomething:NULL>
  <!-- Exception: RuntimeException -->
</file '%s'>
<!-- init Exception::__toString() -->
<Exception::__toString>
  <!-- init Exception::getTraceAsString() -->
  <Exception::getTraceAsString>
  </Exception::getTraceAsString:'#0 %s(%d): fooResults()
#1 %s(%d): doSomething()
#2 {main}'>
</Exception::__toString:'RuntimeException: Oops! in %s%eobserver_generator_%d.php:%d
Stack trace:
#0 %s%eobserver_generator_%d.php(%d): fooResults()
#1 %s%eobserver_generator_%d.php(%d): doSomething()
#2 {main}'>

Fatal error: Uncaught RuntimeException: Oops! in %s%eobserver_generator_%d.php:%d
Stack trace:
#0 %s%eobserver_generator_%d.php(%d): fooResults()
#1 %s%eobserver_generator_%d.php(%d): doSomething()
#2 {main}
  thrown in %s%eobserver_generator_%d.php on line %d

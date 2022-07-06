--TEST--
Observer: Basic observability of userland functions
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
function bar()
{
    echo 'Bar' . PHP_EOL;
    var_dump(array_sum([1,2,3]));
}

function foo()
{
    echo 'Foo' . PHP_EOL;
    bar();
}

foo();
foo();
foo();

echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_basic_01.php' -->
<file '%s%eobserver_basic_01.php'>
  <!-- init foo() -->
  <foo>
Foo
    <!-- init bar() -->
    <bar>
Bar
int(6)
    </bar>
  </foo>
  <foo>
Foo
    <bar>
Bar
int(6)
    </bar>
  </foo>
  <foo>
Foo
    <bar>
Bar
int(6)
    </bar>
  </foo>
DONE
</file '%s%eobserver_basic_01.php'>

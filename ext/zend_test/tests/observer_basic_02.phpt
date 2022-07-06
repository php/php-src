--TEST--
Observer: Basic observability of userland methods
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
class TestClass
{
    private function bar()
    {
        echo 'Bar' . PHP_EOL;
        var_dump(array_sum([1,2,3]));
    }

    public function foo()
    {
        echo 'Foo' . PHP_EOL;
        $this->bar();
    }
}

$test = new TestClass();
$test->foo();
$test->foo();
$test->foo();

echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_basic_02.php' -->
<file '%s%eobserver_basic_02.php'>
  <!-- init TestClass::foo() -->
  <TestClass::foo>
Foo
    <!-- init TestClass::bar() -->
    <TestClass::bar>
Bar
int(6)
    </TestClass::bar>
  </TestClass::foo>
  <TestClass::foo>
Foo
    <TestClass::bar>
Bar
int(6)
    </TestClass::bar>
  </TestClass::foo>
  <TestClass::foo>
Foo
    <TestClass::bar>
Bar
int(6)
    </TestClass::bar>
  </TestClass::foo>
DONE
</file '%s%eobserver_basic_02.php'>

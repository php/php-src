--TEST--
Observer: Basic observability of userland methods
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
opcache.optimization_level=0x7FFFBFFF & ~0x0080
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
<!-- init '%s%eobserver_basic_%d.php' -->
<file '%s%eobserver_basic_%d.php'>
  <!-- init TestClass::foo() -->
  <TestClass::foo>
Foo
    <!-- init TestClass::bar() -->
    <TestClass::bar>
Bar
      <!-- init array_sum() -->
      <array_sum>
      </array_sum>
      <!-- init var_dump() -->
      <var_dump>
int(6)
      </var_dump>
    </TestClass::bar>
  </TestClass::foo>
  <TestClass::foo>
Foo
    <TestClass::bar>
Bar
      <array_sum>
      </array_sum>
      <var_dump>
int(6)
      </var_dump>
    </TestClass::bar>
  </TestClass::foo>
  <TestClass::foo>
Foo
    <TestClass::bar>
Bar
      <array_sum>
      </array_sum>
      <var_dump>
int(6)
      </var_dump>
    </TestClass::bar>
  </TestClass::foo>
DONE
</file '%s%eobserver_basic_%d.php'>

--TEST--
Observer: Basic observability of userland methods
--EXTENSIONS--
zend_test
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
        var_dump(array_reduce([1, 2, 3], fn ($a, $b) => $a + $b));
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
      <!-- init array_reduce() -->
      <array_reduce>
        <!-- init TestClass::{closure}() -->
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
      </array_reduce>
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
      <array_reduce>
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
      </array_reduce>
      <var_dump>
int(6)
      </var_dump>
    </TestClass::bar>
  </TestClass::foo>
  <TestClass::foo>
Foo
    <TestClass::bar>
Bar
      <array_reduce>
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
        <TestClass::{closure}>
        </TestClass::{closure}>
      </array_reduce>
      <var_dump>
int(6)
      </var_dump>
    </TestClass::bar>
  </TestClass::foo>
DONE
</file '%s%eobserver_basic_%d.php'>

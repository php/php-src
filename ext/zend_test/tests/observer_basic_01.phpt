--TEST--
Observer: Basic observability of userland functions
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
opcache.optimization_level=0x7FFFBFFF & ~0x0080
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
<!-- init '%s%eobserver_basic_%d.php' -->
<file '%s%eobserver_basic_%d.php'>
  <!-- init foo() -->
  <foo>
Foo
    <!-- init bar() -->
    <bar>
Bar
      <!-- init array_sum() -->
      <array_sum>
      </array_sum>
      <!-- init var_dump() -->
      <var_dump>
int(6)
      </var_dump>
    </bar>
  </foo>
  <foo>
Foo
    <bar>
Bar
      <array_sum>
      </array_sum>
      <var_dump>
int(6)
      </var_dump>
    </bar>
  </foo>
  <foo>
Foo
    <bar>
Bar
      <array_sum>
      </array_sum>
      <var_dump>
int(6)
      </var_dump>
    </bar>
  </foo>
DONE
</file '%s%eobserver_basic_%d.php'>

--TEST--
Observer: Basic observability of userland functions
--EXTENSIONS--
zend_test
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
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

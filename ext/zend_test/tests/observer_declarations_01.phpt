--TEST--
Observer: Observe function and class declarations
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.observe_declaring=1
--FILE--
<?php
function foo()
{
    echo "foo\n";
}

class A {
}

class B extends A {
}

if (time() > 0) {
    function nested()
    {
    }

    class C
    {
    }

    class D extends A
    {
    }
}

foo();
?>
--EXPECTF--
<!-- declared function 'foo' -->
<!-- declared class 'a' -->
<!-- declared class 'b' -->
<!-- init '%s' -->
<file '%s'>
  <!-- init time() -->
  <time>
  </time>
  <!-- declared function 'nested' -->
  <!-- declared class 'c' -->
  <!-- declared class 'd' -->
  <!-- init foo() -->
  <foo>
foo
  </foo>
</file '%s'>

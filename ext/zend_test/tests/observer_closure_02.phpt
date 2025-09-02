--TEST--
Observer: Observability of fake closures
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php
class Foo
{
    public function bar()
    {
        echo 'Called as fake closure.' . PHP_EOL;
    }
}

$callable = [new Foo(), 'bar'];
$closure = \Closure::fromCallable($callable);
$closure();

echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init Closure::fromCallable() -->
  <Closure::fromCallable>
  </Closure::fromCallable>
  <!-- init Foo::bar() -->
  <Foo::bar>
Called as fake closure.
  </Foo::bar>
DONE
</file '%s'>

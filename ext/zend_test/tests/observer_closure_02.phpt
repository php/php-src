--TEST--
Observer: Observability of fake closures
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
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
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_closure_%d.php' -->
<file '%s%eobserver_closure_%d.php'>
  <!-- init Foo::bar() -->
  <Foo::bar>
Called as fake closure.
  </Foo::bar>
DONE
</file '%s%eobserver_closure_%d.php'>

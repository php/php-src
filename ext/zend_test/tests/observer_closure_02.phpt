--TEST--
Observer: Observability of fake closures
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend_test extension required'); ?>
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
<!-- init '%s%eobserver_closure_%d.php' -->
<file '%s%eobserver_closure_%d.php'>
  <!-- init Foo::bar() -->
  <Foo::bar>
Called as fake closure.
  </Foo::bar>
DONE
</file '%s%eobserver_closure_%d.php'>

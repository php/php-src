--TEST--
Observer: Basic magic method observability
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
class MagicTest
{
    public function __call($name, $args)
    {
        echo '__call()' . PHP_EOL;
        $this->foo($name);
    }

    public function foo($name)
    {
        echo $name . PHP_EOL;
    }
}

$test = new MagicTest();
$test->foo('test');
$test->bar();

echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_magic_01.php' -->
<file '%s%eobserver_magic_01.php'>
  <!-- init MagicTest::foo() -->
  <MagicTest::foo>
test
  </MagicTest::foo>
  <!-- init MagicTest::__call() -->
  <MagicTest::__call>
__call()
    <MagicTest::foo>
bar
    </MagicTest::foo>
  </MagicTest::__call>
DONE
</file '%s%eobserver_magic_01.php'>

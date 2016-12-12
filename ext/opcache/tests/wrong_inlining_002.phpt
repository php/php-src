--TEST--
$this not in object context
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Foo {
    private function getConst() {
        return 42;
    }
    public function test() {
        var_dump($this->getConst());
    }
}

Foo::test();
?>
--EXPECTF--
Deprecated: Non-static method Foo::test() should not be called statically in %swrong_inlining_002.php on line 11

Fatal error: Uncaught Error: Using $this when not in object context in %swrong_inlining_002.php:7
Stack trace:
#0 %swrong_inlining_002.php(11): Foo::test()
#1 {main}
  thrown in %swrong_inlining_002.php on line 7

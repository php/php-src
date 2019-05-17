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
Fatal error: Uncaught Error: Non-static method Foo::test() cannot be called statically in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d

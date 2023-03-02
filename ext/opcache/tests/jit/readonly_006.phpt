--TEST--
JIT readonly modification pre-dec
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
--FILE--
<?php
class Foo {
    public readonly int $bar;

    public function __construct() {
        $this->bar = 1;
        --$this->bar;
    }
}

new Foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property Foo::$bar in %s:%d
Stack trace:
#0 %s(%d): Foo->__construct()
#1 {main}
  thrown in %s on line %d

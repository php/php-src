--TEST--
JIT INIT_STATIC_METHOD_CALL: self:: in an included file keeps the called scope of a static caller
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_loop=1
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    public static function who() {
        return static::class;
    }

    public static function test() {
        include __DIR__ . '/init_static_method_call_002_include.inc';
    }
}

class B extends A {}

B::test();

?>
--EXPECT--
B
B
B

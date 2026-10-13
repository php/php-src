--TEST--
GH-24270 002: Tracing JIT loses the called scope of self:: and parent:: calls in closures
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
--EXTENSIONS--
opcache
--FILE--
<?php
require __DIR__ . '/gh24270.inc';
?>
--EXPECT--
self from static method: Child
bound self: Child
bound parent: GrandChild
trait self A: ChildA
trait self B: ChildB
trait parent A: ChildA
trait parent B: ChildB

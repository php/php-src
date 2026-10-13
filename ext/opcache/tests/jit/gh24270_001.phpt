--TEST--
GH-24270 001: Function JIT loses the called scope of self:: and parent:: calls in closures
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.jit_buffer_size=64M
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

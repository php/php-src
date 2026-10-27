--TEST--
GH-23572: Function JIT resolves self:: in trait methods to the trait
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=1205
opcache.jit_buffer_size=16M
--FILE--
<?php
trait T {
    public function outer() { return self::inner(); }
    public function inner() { return static::class; }
}
class B { use T; }
var_dump((new B)->outer(), (new B)->outer());
?>
--EXPECT--
string(1) "B"
string(1) "B"

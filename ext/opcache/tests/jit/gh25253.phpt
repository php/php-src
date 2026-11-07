--TEST--
GH-25253: JIT reads parent of an unlinked class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.interned_strings_buffer=0
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php

class GH25253ParentClass {
    public function m0() { return 'm0'; }
    public function m1() { return 'm1'; }
    public function m2() { return 'm2'; }
    public function m3() { return 'm3'; }
    public function m4() { return 'm4'; }
    public function m5() { return 'm5'; }
}

require __DIR__ . '/gh25253.inc';

$c = new GH25253Child;
echo $c->m0(), $c->m5(), "\n";

?>
--EXPECT--
m0m5

--TEST--
JIT ASSIGN_STATIC_PROP_OP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
function ref () {
}
class Foo {
      static $i;
      static string $s;
}
Foo::$i = 1;
Foo::$s = Foo::$i;
var_dump(Foo::$s -= ref());
?>
--EXPECT--
string(1) "1"

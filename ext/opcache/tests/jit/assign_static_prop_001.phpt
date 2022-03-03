--TEST--
JIT ASSIGN_STATIC_PROP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
class Foo {
    public static $prop;
}

function test($x) {
    $a = [$x];
    Foo::$prop = $a;
    $a = 42;
}
test(42);
var_dump(Foo::$prop);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}

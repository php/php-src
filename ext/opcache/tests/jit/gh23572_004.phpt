--TEST--
GH-23572: Tracing JIT infers parent:: static property type from the wrong trait clone
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=2
--FILE--
<?php
class ParentA { public static int $x = 42; }
class ParentB { public static string $x = "str"; }
trait T {
    public function get() { return parent::$x; }
}
class A extends ParentA { use T; }
class B extends ParentB { use T; }
var_dump((new A)->get(), (new A)->get(), (new B)->get());
?>
--EXPECT--
int(42)
int(42)
string(3) "str"

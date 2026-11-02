--TEST--
GH-23572: Function JIT calls a class override of a static trait method via self::
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
    public function callMethod() { return self::method(); }
    public static function method() { return 'T'; }
}
class A {
    use T;
    public function method() {
        $a = 1; $b = 2; $c = 3; $d = 4; $e = 5; $f = 6; $g = 7; $h = 8;
        return get_class($this) . ($a + $b + $c + $d + $e + $f + $g + $h);
    }
}
var_dump((new A)->callMethod(), (new A)->callMethod());
?>
--EXPECT--
string(3) "A36"
string(3) "A36"

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
    public function outer($x) { return self::inner($x) + 1; }
    public function inner($x) { return $x * 2; }
}
class A { use T; }
class B { use T; }

$s = 0;
for ($i = 0; $i < 20; $i++) {
    $o = ($i % 2) ? new A : new B;
    try {
        $s += $o->outer($i);
    } catch (Error $e) {
        echo "iter $i: ", $e::class, ": ", $e->getMessage(), "\n";
        break;
    }
}
echo "ok $s\n";
?>
--EXPECT--
ok 400

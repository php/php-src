--TEST--
GH-21267 (JIT infinite loop on FETCH_OBJ_R with IS_UNDEF property in polymorphic context)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=0
opcache.jit_hot_func=2
opcache.jit_hot_return=0
opcache.jit_hot_side_exit=1
--FILE--
<?php
class C {
    public $x = true;
    public function __get($name) { return null; }
    public function getX() { return $this->x; }
}

$o1 = new C;
$o2 = new C;
$o2->x = false;
$o3 = new C;
unset($o3->x);
$a = [$o1, $o2, $o3];

for ($i = 0; $i < 8; $i++) {
    $m = $a[$i % 3];
    $m->getX();
    $m->getX();
}
?>
OK
--EXPECT--
OK

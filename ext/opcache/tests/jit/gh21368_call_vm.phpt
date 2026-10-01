--TEST--
GH-21368 (JIT escape_if_undef SEGV on CALL VM with aggressive trace counters)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.protect_memory=1
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
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

--TEST--
GH-21267 (JIT infinite loop on FETCH_OBJ_R with IS_UNDEF property in polymorphic context)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
--FILE--
<?php
class Base
{
    public $incrementing = true;
    public function __get($name) { return null; }
    public function getIncrementing() { return $this->incrementing; }
    public function getCasts(): array
    {
        if ($this->getIncrementing()) {
            return ['id' => 'int'];
        }
        return [];
    }
}

class ChildA extends Base {}
class ChildB extends Base {}
class ChildC extends Base { public $incrementing = false; }

$classes = [ChildA::class, ChildB::class, ChildC::class];

// Warmup
for ($round = 0; $round < 5; $round++) {
    for ($i = 0; $i < 1000; $i++) {
        $m = new $classes[$i % 3]();
        $m->getIncrementing();
        $m->getCasts();
    }
}

// Trigger
for ($i = 0; $i < 2000; $i++) {
    $m = new $classes[$i % 3]();
    if ($i % 7 === 0) {
        unset($m->incrementing);
    }
    $m->getIncrementing();
    $m->getCasts();
}

echo "OK\n";
--EXPECT--
OK

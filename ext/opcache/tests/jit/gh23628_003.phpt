--TEST--
GH-23628 003: Tracing JIT deoptimization on an unset() property served by __isset()/__get()
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=32M
opcache.jit_hot_loop=16
--EXTENSIONS--
opcache
--FILE--
<?php
class A {
    public $p = ['x' => 1];
    public function __isset($n) { return true; }
    public function __get($n) { return ['x' => 42]; }
    function f($n) {
        $s = 0;
        for ($i = 0; $i < $n; $i++) {
            $s += $this->p['x'] ?? 1000;
        }
        return $s;
    }
}

// The trace is recorded and compiled while the property is initialized...
var_dump((new A)->f(100));

// ... and then executed after the property was unset(), so the fetch has to
// go through __isset()/__get() instead of yielding NULL.
$a = new A;
unset($a->p);
var_dump($a->f(100));
?>
--EXPECT--
int(100)
int(4200)

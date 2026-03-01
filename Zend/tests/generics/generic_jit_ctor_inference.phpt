--TEST--
Generic class: JIT + constructor type inference with --repeat
--DESCRIPTION--
Tests that constructor generic arg inference works correctly when the
constructor is JIT-compiled. The --repeat 2 flag re-executes the script,
and jit_hot_func=1 ensures JIT compilation after the first call.
This caught a real bug where JIT leave helpers didn't call
zend_infer_generic_args_from_constructor().
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// Infer T=int
$box = new Box(42);
echo $box->get() . "\n";

// Must throw TypeError: T=int, passing string
try {
    $box->set("hello");
} catch (TypeError $e) {
    echo "int box: TypeError OK\n";
}

// Infer T=string
$sbox = new Box("world");
echo $sbox->get() . "\n";

// Must throw TypeError: T=string, passing array (non-coercible)
try {
    $sbox->set([1, 2, 3]);
} catch (TypeError $e) {
    echo "string box: TypeError OK\n";
}

// Infer T=float
$fbox = new Box(3.14);
echo $fbox->get() . "\n";

// Hot loop to trigger JIT compilation of constructor
for ($i = 0; $i < 100; $i++) {
    $tmp = new Box($i);
    $tmp->get();
}

// After JIT compilation, inference must still work
$postjit = new Box("after-jit");
try {
    $postjit->set([1]); // array can't coerce to string
} catch (TypeError $e) {
    echo "post-jit: TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
42
int box: TypeError OK
world
string box: TypeError OK
3.14
post-jit: TypeError OK
Done.

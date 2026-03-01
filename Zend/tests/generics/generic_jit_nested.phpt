--TEST--
Generic class: JIT + nested generic objects
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// 1. Nested Box<Box<int>>
$inner = new Box<int>(42);
$outer = new Box<Box<int>>($inner);
echo "1. " . $outer->get()->get() . "\n";

// 2. Hot loop with nested generics
for ($i = 0; $i < 200; $i++) {
    $b = new Box<int>($i);
    $o = new Box<Box<int>>($b);
    $o->get()->get();
}
echo "2. hot nested OK\n";

// 3. Type error on inner type after JIT
$ibox = new Box<int>(10);
$obox = new Box<Box<int>>($ibox);
try {
    $obox->get()->set("wrong"); // inner T=int, passing string
} catch (TypeError $e) {
    echo "3. inner TypeError OK\n";
}

// 4. Type error on outer type after JIT
try {
    $obox->set("not a box"); // outer T=Box<int>, passing string
} catch (TypeError $e) {
    echo "4. outer TypeError OK\n";
}

// 5. Three levels deep
$l1 = new Box<int>(7);
$l2 = new Box<Box<int>>($l1);
$l3 = new Box<Box<Box<int>>>($l2);
echo "5. " . $l3->get()->get()->get() . "\n";

echo "Done.\n";
?>
--EXPECT--
1. 42
2. hot nested OK
3. inner TypeError OK
4. outer TypeError OK
5. 7
Done.

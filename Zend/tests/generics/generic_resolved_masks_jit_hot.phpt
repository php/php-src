--TEST--
Generic class: JIT inline monomorphization with hot loops
--DESCRIPTION--
Tests the JIT inline fast path for generic type checks. The hot loop
triggers JIT compilation, which should use the inline bitmask check
(resolved_masks) instead of calling the C helper for scalar types.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1255
opcache.jit_buffer_size=64M
--FILE--
<?php
declare(strict_types=1);

class NumBox<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// Hot loop: int type — exercises JIT inline mask check for args and returns
$box = new NumBox<int>(0);
$sum = 0;
for ($i = 0; $i < 1000; $i++) {
    $box->set($i);
    $sum += $box->get();
}
echo "int sum: $sum\n";

// Hot loop: string type — exercises JIT inline mask check for string
$sbox = new NumBox<string>("");
$result = "";
for ($i = 0; $i < 100; $i++) {
    $sbox->set("x");
    $result .= $sbox->get();
}
echo "string len: " . strlen($result) . "\n";

// Hot loop: float type
$fbox = new NumBox<float>(0.0);
$fsum = 0.0;
for ($i = 0; $i < 1000; $i++) {
    $fbox->set((float)$i);
    $fsum += $fbox->get();
}
echo "float sum: " . (int)$fsum . "\n";

// Hot loop: property assignment (tests property type check fast path)
$pbox = new NumBox<int>(0);
for ($i = 0; $i < 1000; $i++) {
    $pbox->value = $i;
}
echo "prop final: " . $pbox->value . "\n";

// Hot loop: type error in JIT-compiled code
$errors = 0;
$ebox = new NumBox<int>(0);
for ($i = 0; $i < 100; $i++) {
    try {
        $ebox->set("bad");
    } catch (TypeError $e) {
        $errors++;
    }
}
echo "errors: $errors\n";

// Mixed: alternating between different generic instances in same loop
$ibox = new NumBox<int>(0);
$sbox2 = new NumBox<string>("");
for ($i = 0; $i < 100; $i++) {
    $ibox->set($i);
    $sbox2->set("val$i");
}
echo "mixed int: " . $ibox->get() . "\n";
echo "mixed str: " . $sbox2->get() . "\n";

echo "Done.\n";
?>
--EXPECT--
int sum: 499500
string len: 100
float sum: 499500
prop final: 999
errors: 100
mixed int: 99
mixed str: val99
Done.

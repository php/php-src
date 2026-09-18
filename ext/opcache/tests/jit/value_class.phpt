--TEST--
JIT value class construction, reads and readonly guards preserve semantics
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=16M
opcache.jit=tracing
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
--FILE--
<?php
value class Money {
    public function __construct(public int $amount) {}
    public function double(): int { return $this->amount * 2; }
}
function total(): int {
    $sum = 0;
    for ($i = 0; $i < 100; $i++) {
        $money = new Money($i);
        $sum += $money->amount + $money->double();
    }
    return $sum;
}
for ($i = 0; $i < 20; $i++) {
    if (total() !== 14850) {
        echo "wrong total\n";
    }
}
$money = new Money(100);
try {
    $money->amount++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($money->amount, (new ReflectionClass(Money::class))->isValue());
?>
--EXPECT--
Cannot modify readonly property Money::$amount
int(100)
bool(true)

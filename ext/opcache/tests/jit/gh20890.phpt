--TEST--
GH-20890 (Segfault in zval_undefined_cv with non-simple property hook with minimal tracing JIT)
--CREDITS--
Moonster8282
--EXTENSIONS--
opcache
--INI--
opcache.jit=1251
--FILE--
<?php
class HookJIT {
    private int $readCount = 0;

    public int $computed {
        get {
            $this->readCount++;
            return $this->readCount * 2;
        }
    }
}

function hook_hot_path($obj, $iterations) {
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += $obj->computed;
    }
    return $sum;
}

echo "Testing property hook in hot path...\n";
$obj = new HookJIT();
$result = hook_hot_path($obj, 100);
echo "Result: $result\n";
?>
--EXPECT--
Testing property hook in hot path...
Result: 10100

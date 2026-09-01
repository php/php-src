<?php
/**
 * HydraPHP vs Upstream PHP Benchmark Suite
 * Designed for WatermossMC-like heavy workloads (allocations, loops, hash tables, methods).
 */

$iterations = 500000;

function bench_functions($n) {
    $start = microtime(true);
    for ($i = 0; $i < $n; $i++) {
        strlen("watermossmc");
    }
    return microtime(true) - $start;
}

function bench_loops($n) {
    $start = microtime(true);
    $sum = 0;
    for ($i = 0; $i < $n; $i++) {
        $sum += $i;
    }
    return microtime(true) - $start;
}

function bench_hash_table($n) {
    $start = microtime(true);
    $map = [];
    for ($i = 0; $i < $n; $i++) {
        $map["key_$i"] = $i;
    }
    for ($i = 0; $i < $n; $i++) {
        $val = $map["key_$i"];
    }
    return microtime(true) - $start;
}

class EntityStub {
    public float $x = 0.0;
    public float $y = 0.0;
    public float $z = 0.0;
    public function tick(): void {
        $this->x += 1.0;
    }
}

function bench_objects($n) {
    $start = microtime(true);
    for ($i = 0; $i < $n; $i++) {
        $e = new EntityStub();
        $e->tick();
    }
    return microtime(true) - $start;
}

echo "=== HydraPHP Benchmark Suite ===\n";
echo "PHP Version: " . PHP_VERSION . "\n";
echo "JIT Status: " . (ini_get('opcache.jit') ? 'Enabled' : 'Disabled') . "\n\n";

$t1 = bench_functions($iterations);
$t2 = bench_loops($iterations);
$t3 = bench_hash_table($iterations / 5);
$t4 = bench_objects($iterations / 5);

echo sprintf("Function calls:  %.4f s\n", $t1);
echo sprintf("Loops:           %.4f s\n", $t2);
echo sprintf("HashTable ops:   %.4f s\n", $t3);
echo sprintf("Object ticks:    %.4f s\n", $t4);
echo "=================================\n";
?>

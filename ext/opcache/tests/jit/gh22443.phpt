--TEST--
GH-22443 (SIGSEGV in tracing JIT: run_time_cache offset stored without map_ptr dereference)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_max_polymorphic_calls=0
--FILE--
<?php
final class Svc {
    public function m0(...$a): int { return (int) array_sum($a) + 0; }
    public function m1(...$a): int { return (int) array_sum($a) + 1; }
    public function m2(...$a): int { return (int) array_sum($a) + 2; }
    public function m3(...$a): int { return (int) array_sum($a) + 3; }
    public function m4(...$a): int { return (int) array_sum($a) + 4; }
    public function m5(...$a): int { return (int) array_sum($a) + 5; }
    public function m6(...$a): int { return (int) array_sum($a) + 6; }
    public function m7(...$a): int { return (int) array_sum($a) + 7; }
    public function coldMethod(...$a): int { return $this->helper((int) array_sum($a)); }
    private function helper(int $x): int { return $x + 1; }
}

function makeListener($listener): Closure {
    return function ($event, $payload) use ($listener) {
        return $listener(...array_values($payload));
    };
}

function invokeListeners(array $listeners, string $event, array $payload): int {
    $r = 0;
    foreach ($listeners as $l) {
        $r += $l($event, $payload);
    }
    return $r;
}

$svc = new Svc();
$warm = [];
for ($k = 0; $k < 8; $k++) {
    $warm[] = makeListener([$svc, 'm' . $k]);
}
$cold = [makeListener([$svc, 'coldMethod'])];

$s = 0;
for ($i = 0; $i < 400; $i++) {
    $s += invokeListeners($warm, 'e', [$i & 7, ($i >> 2) & 7]);
}
for ($j = 0; $j < 5; $j++) {
    $s += invokeListeners($cold, 'e', [$j, $j + 1]);
}
echo "done\n";
?>
--EXPECT--
done

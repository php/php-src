--TEST--
Benchmark array_str_contains vs userland implementations
--FILE--
<?php

$iterations = 100_000;
$haystack_start = array_merge(['apple'], array_fill(0, 50, 'banana'));
$haystack_mid   = array_merge(array_fill(0, 25, 'banana'), ['apple'], array_fill(0, 25, 'banana'));
$haystack_end   = array_merge(array_fill(0, 50, 'banana'), ['apple']);
$haystack_none  = array_fill(0, 50, 'banana');

$cases = [
    'Match at start'  => $haystack_start,
    'Match in middle' => $haystack_mid,
    'Match at end'    => $haystack_end,
    'No match (all)'  => $haystack_none,
];

echo "=== BENCHMARK RESULTS (" . number_format($iterations) . " iterations) ===\n\n";

foreach ($cases as $label => $array) {
    echo "[$label]\n";

    // 1. C implementation (array_str_contains)
    $t1 = hrtime(true);
    for ($i = 0; $i < $iterations; $i++) {
        $res1 = array_str_contains($array, 'apple');
    }
    $c_time = (hrtime(true) - $t1) / 1e6; // ms

    // 2. Userland foreach + str_contains
    $t2 = hrtime(true);
    for ($i = 0; $i < $iterations; $i++) {
        $res2 = false;
        foreach ($array as $item) {
            if (is_string($item) && str_contains($item, 'apple')) {
                $res2 = true;
                break;
            }
        }
    }
    $userland_time = (hrtime(true) - $t2) / 1e6; // ms

    $speedup = ($userland_time / $c_time);

    printf("  C (array_str_contains) : %7.2f ms\n", $c_time);
    printf("  Userland (foreach)     : %7.2f ms\n", $userland_time);
    printf("  Speedup                : %7.2fx faster\n\n", $speedup);
}
?>
--EXPECTF--
=== BENCHMARK RESULTS (%s iterations) ===
%a

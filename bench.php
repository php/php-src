<?php

function generate_values(int $n) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, $n - 1);
    }
    return $values;
}

function bench_array_unique_identical(int $n, int $iterations) {
    $values = generate_values($n);
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum(array_unique($values, ARRAY_UNIQUE_IDENTICAL));
    }
    $end = hrtime(true);
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}

$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 8000000],
    [4, 2000000],
    [8, 1000000],
    [2**10, 20000],
    [2**20, 20],
];

foreach ($sizes as [$n, $iterations]) {
    bench_array_unique_identical($n, $iterations);
}

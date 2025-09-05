<?php

$size = 1000000;
$iterations = 10;

echo "Benchmarking array_sum with an array of $size integers, $iterations iterations...\n";

$total_time = 0;

for ($i = 0; $i < $iterations; $i++) {
    $array = range(0, $size - 1);
    $start_time = hrtime(true);
    $sum = array_sum($array);
    $end_time = hrtime(true);
    $total_time += $end_time - $start_time;
}

$avg_time = $total_time / $iterations;

echo "Average execution time: " . ($avg_time / 1e9) . " seconds\n";

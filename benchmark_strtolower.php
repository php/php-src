<?php

function benchmark_strtolower(string $str) {
    $start = microtime(true);
    for ($i = 0; $i < 100000; $i++) {
        strtolower($str);
    }
    $end = microtime(true);
    return $end - $start;
}

$short_ascii = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
$long_ascii = str_repeat($short_ascii, 1000);
$short_mixed = 'aBcDeFgHiJkLmNoPqRsTuVwXyZ';
$long_mixed = str_repeat($short_mixed, 1000);
$short_lower = 'abcdefghijklmnopqrstuvwxyz';
$long_lower = str_repeat($short_lower, 1000);

echo "Benchmarking current strtolower implementation...\n";

echo "Short ASCII string: " . benchmark_strtolower($short_ascii) . "s\n";
echo "Long ASCII string: " . benchmark_strtolower($long_ascii) . "s\n";
echo "Short mixed-case string: " . benchmark_strtolower($short_mixed) . "s\n";
echo "Long mixed-case string: " . benchmark_strtolower($long_mixed) . "s\n";
echo "Short lowercase string: " . benchmark_strtolower($short_lower) . "s\n";
echo "Long lowercase string: " . benchmark_strtolower($long_lower) . "s\n";

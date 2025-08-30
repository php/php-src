<?php

function benchmark_strtoupper(string $str) {
    $start = microtime(true);
    for ($i = 0; $i < 100000; $i++) {
        strtoupper($str);
    }
    $end = microtime(true);
    return $end - $start;
}

$short_ascii = 'abcdefghijklmnopqrstuvwxyz';
$long_ascii = str_repeat($short_ascii, 1000);
$short_mixed = 'aBcDeFgHiJkLmNoPqRsTuVwXyZ';
$long_mixed = str_repeat($short_mixed, 1000);
$short_upper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
$long_upper = str_repeat($short_upper, 1000);

echo "Benchmarking current strtoupper implementation...\n";

echo "Short ASCII string: " . benchmark_strtoupper($short_ascii) . "s\n";
echo "Long ASCII string: " . benchmark_strtoupper($long_ascii) . "s\n";
echo "Short mixed-case string: " . benchmark_strtoupper($short_mixed) . "s\n";
echo "Long mixed-case string: " . benchmark_strtoupper($long_mixed) . "s\n";
echo "Short uppercase string: " . benchmark_strtoupper($short_upper) . "s\n";
echo "Long uppercase string: " . benchmark_strtoupper($long_upper) . "s\n";

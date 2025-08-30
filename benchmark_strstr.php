<?php

function benchmark_strstr(string $haystack, string $needle) {
    $start = microtime(true);
    for ($i = 0; $i < 100000; $i++) {
        strstr($haystack, $needle);
    }
    $end = microtime(true);
    return $end - $start;
}

$long_haystack = str_repeat('abcdefghijklmnopqrstuvwxyz', 1000);
$short_needle_front = 'abc';
$short_needle_middle = 'mno';
$short_needle_end = 'xyz';
$not_found_needle = '123';

echo "Benchmarking current strstr implementation...\n";

echo "Long haystack, short needle (front): " . benchmark_strstr($long_haystack, $short_needle_front) . "s\n";
echo "Long haystack, short needle (middle): " . benchmark_strstr($long_haystack, $short_needle_middle) . "s\n";
echo "Long haystack, short needle (end): " . benchmark_strstr($long_haystack, $short_needle_end) . "s\n";
echo "Long haystack, needle not found: " . benchmark_strstr($long_haystack, $not_found_needle) . "s\n";

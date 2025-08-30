<?php

function benchmark_str_pad(string $input, int $pad_length, string $pad_string, int $pad_type) {
    $start = microtime(true);
    for ($i = 0; $i < 100000; $i++) {
        str_pad($input, $pad_length, $pad_string, $pad_type);
    }
    $end = microtime(true);
    return $end - $start;
}

$short_string = 'Hello';
$long_string = str_repeat('Hello', 100);

echo "Benchmarking current str_pad implementation...\n";

echo "Short string, right pad: " . benchmark_str_pad($short_string, 100, ' ', STR_PAD_RIGHT) . "s\n";
echo "Short string, left pad: " . benchmark_str_pad($short_string, 100, ' ', STR_PAD_LEFT) . "s\n";
echo "Short string, both pad: " . benchmark_str_pad($short_string, 100, ' ', STR_PAD_BOTH) . "s\n";

echo "Long string, right pad: " . benchmark_str_pad($long_string, 1000, ' ', STR_PAD_RIGHT) . "s\n";
echo "Long string, left pad: " . benchmark_str_pad($long_string, 1000, ' ', STR_PAD_LEFT) . "s\n";
echo "Long string, both pad: " . benchmark_str_pad($long_string, 1000, ' ', STR_PAD_BOTH) . "s\n";

echo "Short string, multi-char pad, right: " . benchmark_str_pad($short_string, 100, '-=', STR_PAD_RIGHT) . "s\n";
echo "Short string, multi-char pad, left: " . benchmark_str_pad($short_string, 100, '-=', STR_PAD_LEFT) . "s\n";
echo "Short string, multi-char pad, both: " . benchmark_str_pad($short_string, 100, '-=', STR_PAD_BOTH) . "s\n";

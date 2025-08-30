<?php

function benchmark_str_word_count(string $string, int $format = 0, ?string $characters = null) {
    $start = microtime(true);
    for ($i = 0; $i < 100000; $i++) {
        str_word_count($string, $format, $characters);
    }
    $end = microtime(true);
    return $end - $start;
}

$short_string = 'Hello world, this is a test.';
$long_string = str_repeat($short_string . ' ', 1000);

echo "Benchmarking current str_word_count implementation...\n";

echo "Short string, format 0: " . benchmark_str_word_count($short_string, 0) . "s\n";
echo "Short string, format 1: " . benchmark_str_word_count($short_string, 1) . "s\n";
echo "Short string, format 2: " . benchmark_str_word_count($short_string, 2) . "s\n";

echo "Long string, format 0: " . benchmark_str_word_count($long_string, 0) . "s\n";
echo "Long string, format 1: " . benchmark_str_word_count($long_string, 1) . "s\n";
echo "Long string, format 2: " . benchmark_str_word_count($long_string, 2) . "s\n";

echo "Short string with extra chars, format 0: " . benchmark_str_word_count($short_string, 0, '.,') . "s\n";
echo "Long string with extra chars, format 0: " . benchmark_str_word_count($long_string, 0, '.,') . "s\n";

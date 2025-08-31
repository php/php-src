<?php
$iterations = 1000000;
$data = 'The quick brown fox jumps over the lazy dog';

$start = microtime(true);
for ($i = 0; $i < $iterations; $i++) {
    md5($data);
}
$end = microtime(true);

echo "MD5 benchmark (1,000,000 iterations):\n";
echo "Total time: " . ($end - $start) . " seconds\n";
?>

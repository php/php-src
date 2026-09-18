<?php
declare(strict_types=1);

function median(array $values): float
{
    sort($values, SORT_NUMERIC);
    $middle = intdiv(count($values), 2);
    return count($values) % 2 ? (float) $values[$middle] : ($values[$middle - 1] + $values[$middle]) / 2;
}

function positiveCount(string $value, int $minimum = 1): int
{
    $number = (int) $value;
    if ($number < $minimum || (string) $number !== $value) {
        throw new InvalidArgumentException('Usage: php run.php [iterations > 0] [repeats >= 3] [instances > 0]');
    }
    return $number;
}
$iterations = positiveCount($argv[1] ?? '1000000');
$repeats = positiveCount($argv[2] ?? '7', 3);
$instances = positiveCount($argv[3] ?? '200000');

$kinds = ['regular', 'readonly', 'value'];
$samples = [];
foreach (['construct', 'read', 'method', 'memory'] as $metric) {
    for ($round = 0; $round < $repeats; $round++) {
        // Rotate ordering and use fresh processes to avoid allocator/history bias.
        for ($offset = 0; $offset < count($kinds); $offset++) {
            $kind = $kinds[($round + $offset) % count($kinds)];
            $command = [
                PHP_BINARY, '-n', '-d', 'opcache.enable_cli=0', '-d', 'opcache.jit=disable',
                __DIR__ . '/worker.php', $kind, $metric,
                (string) ($metric === 'memory' ? $instances : $iterations),
            ];
            $process = proc_open($command, [1 => ['pipe', 'w'], 2 => ['pipe', 'w']], $pipes);
            if (!is_resource($process)) {
                throw new RuntimeException('Unable to start benchmark worker');
            }
            $output = stream_get_contents($pipes[1]);
            $error = stream_get_contents($pipes[2]);
            fclose($pipes[1]);
            fclose($pipes[2]);
            if (proc_close($process) !== 0 || $error !== '') {
                throw new RuntimeException("Worker failed: $error$output");
            }
            $samples[$metric][$kind][] = json_decode($output, true, flags: JSON_THROW_ON_ERROR);
        }
    }
}

$summary = [];
foreach ($samples as $metric => $groups) {
    foreach ($groups as $kind => $runs) {
        $field = $metric === 'memory' ? 'bytes_per_instance' : 'ns_per_op';
        $values = array_column($runs, $field);
        $summary[$metric][$kind] = [
            'unit' => $metric === 'memory' ? 'bytes/instance' : 'ns/op',
            'median' => median($values),
            'min' => min($values),
            'max' => max($values),
        ];
    }
}
echo json_encode([
    'environment' => [
        'php' => PHP_VERSION,
        'binary' => PHP_BINARY,
        'debug' => (bool) PHP_DEBUG,
        'zts' => (bool) PHP_ZTS,
        'os' => php_uname(),
        'opcache' => false,
        'jit' => false,
    ],
    'iterations' => $iterations,
    'repeats' => $repeats,
    'instances' => $instances,
    'summary' => $summary,
    'samples' => $samples,
], JSON_PRETTY_PRINT | JSON_THROW_ON_ERROR), "\n";

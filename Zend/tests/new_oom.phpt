--TEST--
Test OOM on new of each instantiable class
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires zmm");
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php

function getOomProcessCount(): int
{
    $processCount = PHP_OS_FAMILY === 'Windows'
        ? getenv('NUMBER_OF_PROCESSORS')
        : shell_exec('getconf _NPROCESSORS_ONLN 2>/dev/null');
    if (!is_string($processCount)) {
        return 1;
    }

    $processCount = trim($processCount);
    if (preg_match('/^[1-9][0-9]*$/D', $processCount) !== 1) {
        return 1;
    }
    return min((int) $processCount, 4);
}

function startOomTest(string $php, string $file, string $class): ?array
{
    $output = tmpfile();
    if ($output === false) {
        echo "Class $class failed\nUnable to create output file\n";
        return null;
    }

    $process = proc_open(
        [$php, '--no-php-ini', $file, $class],
        [
            0 => ['null'],
            1 => $output,
            2 => ['redirect', 1],
        ],
        $pipes,
    );
    if (!is_resource($process)) {
        fclose($output);
        echo "Class $class failed\nUnable to start process\n";
        return null;
    }

    return [
        'class' => $class,
        'output' => $output,
        'process' => $process,
    ];
}

function finishOomTest(array $test): bool
{
    $status = proc_get_status($test['process']);
    if ($status['running']) {
        return false;
    }

    proc_close($test['process']);
    rewind($test['output']);
    $output = stream_get_contents($test['output']);
    fclose($test['output']);

    if ($status['signaled']) {
        echo "Class {$test['class']} failed\n";
        echo "Process terminated by signal {$status['termsig']}\n";
    } elseif ($output && preg_match('(^\nFatal error: Allowed memory size of [0-9]+ bytes exhausted[^\r\n]* \(tried to allocate [0-9]+ bytes\) in [^\r\n]+ on line [0-9]+\nStack trace:\n(#[0-9]+ [^\r\n]+\n)+$)', $output) !== 1) {
        echo "Class {$test['class']} failed\n";
        echo $output, "\n";
    }

    return true;
}

$file = __DIR__ . '/new_oom.inc';
$php = PHP_BINARY;
$classes = array_filter(
    get_declared_classes(),
    static fn(string $class): bool => (new ReflectionClass($class))->isInstantiable(),
);
$tests = [];
$processCount = getOomProcessCount();

while ($classes || $tests) {
    while ($classes && count($tests) < $processCount) {
        $class = array_shift($classes);
        $test = startOomTest($php, $file, $class);
        if ($test !== null) {
            $tests[] = $test;
        }
    }

    $testFinished = false;
    foreach ($tests as $index => $test) {
        if (finishOomTest($test)) {
            unset($tests[$index]);
            $testFinished = true;
        }
    }
    if (!$testFinished) {
        usleep(1000);
    }
}

?>
===DONE===
--EXPECT--
===DONE===

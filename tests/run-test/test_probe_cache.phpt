--TEST--
Shared test probe cache caches failures across processes
--FILE--
<?php
require dirname(__DIR__) . '/probe_cache.inc';

function start_probe_cache_process(string $code, array $environment): array
{
    $command = getenv('TEST_PHP_EXECUTABLE_ESCAPED')
        . ' -n -r '
        . escapeshellarg($code);
    $process = proc_open(
        $command,
        [
            1 => ['pipe', 'w'],
            2 => ['redirect', 1],
        ],
        $pipes,
        null,
        $environment,
        ['bypass_shell' => true],
    );

    return [$process, $pipes];
}

function finish_probe_cache_process($process, array $pipes): string
{
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);

    if (0 !== $exitCode = proc_close($process)) {
        throw new Exception("PHP subprocess exited with code $exitCode: $output");
    }

    return $output;
}

function run_probe_cache_process(string $code, array $environment): string
{
    [$process, $pipes] = start_probe_cache_process($code, $environment);
    return finish_probe_cache_process($process, $pipes);
}

$cacheDirectory = getenv('TEST_PHP_SHARED_CACHE_DIR');
if (!is_string($cacheDirectory)) {
    throw new Exception('Missing shared test cache directory');
}

$environment = getenv();

$helper = var_export(dirname(__DIR__) . '/probe_cache.inc', true);
$namespace = 'probe-cache-test-' . bin2hex(random_bytes(8));
$namespaceCode = var_export($namespace, true);
$first = run_probe_cache_process(
    "require $helper; echo ProbeCache::getFailure($namespaceCode, ['shared'], static fn(): ?string => 'shared failure');",
    $environment,
);
$second = run_probe_cache_process(
    "require $helper; echo ProbeCache::getFailure($namespaceCode, ['shared'], static function (): ?string { throw new Exception('Probe should not run'); });",
    $environment,
);
echo "$first\n$second\n";

$probeStarted = $cacheDirectory . '/probe_started';
$probeStartedCode = var_export($probeStarted, true);
@unlink($probeStarted);
[$firstProcess, $firstPipes] = start_probe_cache_process(
    "require $helper; echo ProbeCache::getFailure($namespaceCode, ['concurrent'], static function (): ?string { file_put_contents($probeStartedCode, 'started'); usleep(1000000); return 'concurrent failure'; });",
    $environment,
);

$deadline = microtime(true) + 5;
while (!file_exists($probeStarted) && microtime(true) < $deadline) {
    usleep(1000);
}
if (!file_exists($probeStarted)) {
    $output = finish_probe_cache_process($firstProcess, $firstPipes);
    throw new Exception("Concurrent probe did not start: $output");
}

[$secondProcess, $secondPipes] = start_probe_cache_process(
    "require $helper; echo ProbeCache::getFailure($namespaceCode, ['concurrent'], static function (): ?string { throw new Exception('Concurrent probe should not run'); });",
    $environment,
);
$first = finish_probe_cache_process($firstProcess, $firstPipes);
$second = finish_probe_cache_process($secondProcess, $secondPipes);
echo "$first\n$second\n";

putenv("TEST_PHP_SHARED_CACHE_DIR=$cacheDirectory");

$failureCalls = 0;
$failureProbe = static function () use (&$failureCalls): ?string {
    $failureCalls++;
    return "failure $failureCalls";
};

var_dump(ProbeCache::getFailure($namespace, ['first'], $failureProbe));
var_dump(ProbeCache::getFailure($namespace, ['first'], $failureProbe));
var_dump(ProbeCache::getFailure($namespace, ['second'], $failureProbe));
var_dump($failureCalls);

$successCalls = 0;
$successProbe = static function () use (&$successCalls): ?string {
    $successCalls++;
    return null;
};

var_dump(ProbeCache::getFailure($namespace, ['available'], $successProbe));
var_dump(ProbeCache::getFailure($namespace, ['available'], $successProbe));
var_dump($successCalls);

putenv('TEST_PHP_SHARED_CACHE_DIR');
var_dump(ProbeCache::getFailure($namespace, ['uncached'], static fn(): ?string => 'uncached failure'));
?>
--EXPECT--
shared failure
shared failure
concurrent failure
concurrent failure
string(9) "failure 1"
string(9) "failure 1"
string(9) "failure 2"
int(2)
NULL
NULL
int(2)
string(16) "uncached failure"

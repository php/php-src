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
$environment['TEST_PHP_EVALUATING_SKIPIF'] = '1';

$helper = var_export(dirname(__DIR__) . '/probe_cache.inc', true);
$namespace = 'probe-cache-test-' . bin2hex(random_bytes(8));
$namespaceCode = var_export($namespace, true);
$first = run_probe_cache_process(
    "require $helper; try { ProbeCache::getFailure($namespaceCode, ['shared'], static function (): never { throw new ProbeFailureException('shared failure'); }); } catch (ProbeFailureException \$e) { echo \$e->getMessage(); }",
    $environment,
);
$second = run_probe_cache_process(
    "require $helper; try { ProbeCache::getFailure($namespaceCode, ['shared'], static function (): never { throw new Exception('Probe should not run'); }); } catch (ProbeFailureException \$e) { echo \$e->getMessage(); }",
    $environment,
);
echo "$first\n$second\n";

$probeStarted = $cacheDirectory . '/probe_started';
$probeStartedCode = var_export($probeStarted, true);
@unlink($probeStarted);
[$firstProcess, $firstPipes] = start_probe_cache_process(
    "require $helper; try { ProbeCache::getFailure($namespaceCode, ['concurrent'], static function (): never { file_put_contents($probeStartedCode, 'started'); usleep(1000000); throw new ProbeFailureException('concurrent failure'); }); } catch (ProbeFailureException \$e) { echo \$e->getMessage(); }",
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
    "require $helper; try { ProbeCache::getFailure($namespaceCode, ['concurrent'], static function (): never { throw new Exception('Concurrent probe should not run'); }); } catch (ProbeFailureException \$e) { echo \$e->getMessage(); }",
    $environment,
);
$first = finish_probe_cache_process($firstProcess, $firstPipes);
$second = finish_probe_cache_process($secondProcess, $secondPipes);
echo "$first\n$second\n";

putenv("TEST_PHP_SHARED_CACHE_DIR=$cacheDirectory");
putenv('TEST_PHP_EVALUATING_SKIPIF=1');

$failureCalls = 0;
$failureProbe = static function () use (&$failureCalls): never {
    $failureCalls++;
    throw new ProbeFailureException("failure $failureCalls");
};

try {
    ProbeCache::getFailure($namespace, ['first'], $failureProbe);
} catch (ProbeFailureException $e) {
    var_dump($e->getMessage());
}
try {
    ProbeCache::getFailure($namespace, ['first'], $failureProbe);
} catch (ProbeFailureException $e) {
    var_dump($e->getMessage());
}
try {
    ProbeCache::getFailure($namespace, ['second'], $failureProbe);
} catch (ProbeFailureException $e) {
    var_dump($e->getMessage());
}
var_dump($failureCalls);

$successCalls = 0;
$successProbe = static function () use (&$successCalls): string {
    $successCalls++;
    return "success $successCalls";
};

var_dump(ProbeCache::getFailure($namespace, ['available'], $successProbe));
var_dump(ProbeCache::getFailure($namespace, ['available'], $successProbe));
var_dump($successCalls);

putenv('TEST_PHP_EVALUATING_SKIPIF');
var_dump(ProbeCache::getFailure($namespace, ['first'], static fn(): string => 'uncached success'));

$previous = new Exception('original failure');
try {
    ProbeCache::getFailure($namespace, ['wrapped'], static function () use ($previous): never {
        throw new ProbeFailureException($previous);
    });
} catch (Throwable $e) {
    var_dump($e === $previous);
}

putenv('TEST_PHP_EVALUATING_SKIPIF=1');
putenv('TEST_PHP_SHARED_CACHE_DIR');
try {
    ProbeCache::getFailure($namespace, ['uncached'], static function (): never {
        throw new ProbeFailureException('uncached failure');
    });
} catch (ProbeFailureException $e) {
    var_dump($e->getMessage());
}
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
string(9) "success 1"
string(9) "success 2"
int(2)
string(16) "uncached success"
bool(true)
string(16) "uncached failure"

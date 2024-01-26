<?php

require_once __DIR__ . '/shared.php';

$storeResult = ($argv[1] ?? 'false') === 'true';
$phpCgi = $argv[2] ?? dirname(PHP_BINARY) . '/php-cgi';
if (!file_exists($phpCgi)) {
    fwrite(STDERR, "php-cgi not found\n");
    exit(1);
}

function main() {
    global $storeResult;

    $profilesDir = __DIR__ . '/profiles';
    if (!is_dir($profilesDir)) {
        mkdir($profilesDir, 0755, true);
    }

    $data = [];
    if (false !== $branch = getenv('GITHUB_REF_NAME')) {
        $data['branch'] = $branch;
    }
    $data['Zend/bench.php'] = runBench(false);
    $data['Zend/bench.php JIT'] = runBench(true);
    $data['Symfony Demo 2.2.3'] = runSymfonyDemo(false);
    $data['Symfony Demo 2.2.3 JIT'] = runSymfonyDemo(true);
    $data['Wordpress 6.2'] = runWordpress(false);
    $data['Wordpress 6.2 JIT'] = runWordpress(true);
    $result = json_encode($data, JSON_PRETTY_PRINT) . "\n";

    fwrite(STDOUT, $result);

    if ($storeResult) {
        storeResult($result);
    }
}

function storeResult(string $result) {
    $repo = __DIR__ . '/repos/data';
    cloneRepo($repo, 'git@github.com:php/benchmarking-data.git');

    $commitHash = getPhpSrcCommitHash();
    $dir = $repo . '/' . substr($commitHash, 0, 2) . '/' . $commitHash;
    $summaryFile = $dir . '/summary.json';
    if (!is_dir($dir)) {
        mkdir($dir, 0755, true);
    }
    file_put_contents($summaryFile, $result);
}

function getPhpSrcCommitHash(): string {
    $result = runCommand(['git', 'log', '--pretty=format:%H', '-n', '1'], dirname(__DIR__));
    return $result->stdout;
}

function runBench(bool $jit): array {
    return runValgrindPhpCgiCommand('bench', [dirname(__DIR__) . '/Zend/bench.php'], jit: $jit);
}

function runSymfonyDemo(bool $jit): array {
    $dir = __DIR__ . '/repos/symfony-demo-2.2.3';
    cloneRepo($dir, 'https://github.com/php/benchmarking-symfony-demo-2.2.3.git');
    runPhpCommand([$dir . '/bin/console', 'cache:clear']);
    runPhpCommand([$dir . '/bin/console', 'cache:warmup']);

    return runValgrindPhpCgiCommand('symfony-demo', [$dir . '/public/index.php'], cwd: $dir, jit: $jit, repeat: 100);
}

function runWordpress(bool $jit): array {
    $dir = __DIR__ . '/repos/wordpress-6.2';
    cloneRepo($dir, 'https://github.com/php/benchmarking-wordpress-6.2.git');

    /* FIXME: It might be better to use a stable version of PHP for this command because we can't
     * easily alter the phar file */
    runPhpCommand([
        '-d error_reporting=0',
        'wp-cli.phar',
        'core',
        'install',
        '--url=wordpress.local',
        '--title="Wordpress"',
        '--admin_user=wordpress',
        '--admin_password=wordpress',
        '--admin_email=benchmark@php.net',
    ], $dir);

    // Warmup
    runPhpCommand([$dir . '/index.php'], $dir);

    return runValgrindPhpCgiCommand('wordpress', [$dir . '/index.php'], cwd: $dir, jit: $jit, repeat: 100);
}

function runPhpCommand(array $args, ?string $cwd = null): ProcessResult {
    return runCommand([PHP_BINARY, ...$args], $cwd);
}

function runValgrindPhpCgiCommand(
    string $name,
    array $args,
    ?string $cwd = null,
    bool $jit = false,
    int $repeat = 1,
): array {
    global $phpCgi;

    $profileOut = __DIR__ . "/profiles/callgrind.$name";
    if ($jit) {
        $profileOut .= '-jit';
    }

    $process = runCommand([
        'valgrind',
        '--tool=callgrind',
        '--dump-instr=yes',
        '--collect-jumps=yes',
        '--trace-children=yes',
        "--callgrind-out-file=$profileOut",
        '--verbose',
        '--',
        $phpCgi,
        '-T' . $repeat,
        '-d max_execution_time=0',
        '-d opcache.enable=1',
        '-d opcache.jit=' . ($jit ? 'tracing' : 'disable'),
        '-d opcache.jit_buffer_size=128M',
        '-d opcache.validate_timestamps=0',
        ...$args,
    ], null, array_merge(getenv(), ['BENCHMARK_DUMP_SEPARATE_PROFILES' => '1']));

    // collect metrics for startup, each benchmark run and shutdown
    $metricsArr = [];
    foreach (['startup' => 1, ...range(2, $repeat + 1), 'shutdown' => ''] as $k => $kCallgrindOut) {
        $profileOutSpecific = $profileOut . '.' . $k;
        rename($profileOut . ($kCallgrindOut === '' ? '' : '.' . $kCallgrindOut), $profileOutSpecific);

        $metricsArr[$k] = extractMetricsFromCallgrindFile($profileOutSpecific);
    }

    // print all collected metrics
    print_r(array_map(fn ($metrics) => array_map(fn ($v) => number_format($v, 0, '.', '_'), $metrics), $metricsArr));

    // find median benchmark run
    $metricsForMedianArr = $metricsArr;
    unset($metricsForMedianArr['startup']);
    unset($metricsForMedianArr['shutdown']);
    uasort($metricsForMedianArr, fn ($a, $b) => $a['Ir'] <=> $b['Ir']);
    $medianRunIndex = array_keys($metricsForMedianArr)[max(0, floor((count($metricsForMedianArr) - 3 /* -1 for count to index, -1 for first slow run due compliation, -1 for second run which is a little slower too */) / 2.0))];

    // remove non-median profiles from artifacts
    foreach (range(0, $repeat - 1) as $k) {
        $profileOutSpecific = $profileOut . '.' . $k;

        if ($k !== $medianRunIndex) {
            unlink($profileOutSpecific);
        }
    }

    // annotate profiles for artifacts
    foreach (['startup', $medianRunIndex, 'shutdown'] as $k) {
        $profileOutSpecific = $profileOut . '.' . $k;

        runCommand([
            'callgrind_annotate',
            '--threshold=100',
            '--auto=yes',
            '--show-percs=no',
            $profileOutSpecific,
            new UnescapedArg('>'),
            "$profileOutSpecific.txt",
        ]);
    }

    $instructions = $metricsArr[$medianRunIndex]['Ir'];
    if ($repeat === 1) { // return the same data as before https://github.com/php/php-src/pull/13162
        $instructions += $metricsArr['startup']['Ir'];
    }

    return ['instructions' => $instructions];
}

/**
 * @return array<non-empty-string, numeric-string>
 */
function extractMetricsFromCallgrindFile(string $path): array {
    if (!preg_match('/\nevents:((?: +\w+)+)\nsummary:((?: +\d+)+)\n/', file_get_contents($path, length: 10_000), $matches)) {
        throw new \Exception('Unexpected callgrind data');
    }

    return array_combine(explode(' ', ltrim($matches[1], ' ')), explode(' ', ltrim($matches[2], ' ')));
}

main();

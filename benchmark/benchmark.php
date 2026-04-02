<?php

require_once __DIR__ . '/shared.php';

checkExtensions(['gmp']);

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

    checkExtensions(['mbstring']);
    $data['Symfony Demo 2.2.3'] = runSymfonyDemo(false);
    $data['Symfony Demo 2.2.3 JIT'] = runSymfonyDemo(true);

    checkExtensions(['mbstring', 'sockets', 'mysqli', 'openssl']);
    $data['Wordpress 6.2'] = runWordpress(false);
    $data['Wordpress 6.2 JIT'] = runWordpress(true);

    $result = json_encode($data, JSON_PRETTY_PRINT) . "\n";

    fwrite(STDOUT, $result);

    if ($storeResult) {
        storeResult($result);
    }
}

function checkExtensions(array $extensions): void {
    foreach ($extensions as $ext) {
        if (!extension_loaded($ext)) {
            throw new LogicException("Extension $ext is required.");
        }
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

    $profileOut = __DIR__ . "/profiles/callgrind.out.$name";
    if ($jit) {
        $profileOut .= '.jit';
    }

    $process = runCommand([
        'valgrind',
        '--tool=callgrind',
        '--dump-instr=yes',
        "--callgrind-out-file=$profileOut",
        '--',
        $phpCgi,
        '-T' . $repeat,
        '-d max_execution_time=0',
        '-d opcache.enable=1',
        '-d opcache.jit=' . ($jit ? 'tracing' : 'disable'),
        '-d opcache.jit_buffer_size=128M',
        '-d opcache.validate_timestamps=0',
        ...$args,
    ]);
    $totals = extractTotalsFromCallgrindFile($profileOut);
    $instructions = $totals['Ir'];
    if ($repeat > 1) {
        $instructions = gmp_strval(gmp_div_q($instructions, $repeat));
    }
    return ['instructions' => $instructions];
}

/**
 * @return non-empty-array<non-empty-string, decimal-int-string>
 */
function extractTotalsFromCallgrindFile(string $file): array {
    $data = file_get_contents($file);

    if (!preg_match_all('(\nevents:((?: +\w+)+)\n)', $data, $matchesAll, \PREG_SET_ORDER)) {
        throw new \Exception('Unexpected callgrind data - "events" not found');
    }
    $events = preg_split('( +)', ltrim(array_last($matchesAll)[1]), -1, \PREG_SPLIT_NO_EMPTY);

    if (!preg_match_all('(\ntotals:((?: +\w+)+)\n)', $data, $matchesAll, \PREG_SET_ORDER)) {
        throw new \Exception('Unexpected callgrind data - "totals" not found');
    }
    $totals = preg_split('( +)', ltrim(array_last($matchesAll)[1]), -1, \PREG_SPLIT_NO_EMPTY);

    return array_combine($events, $totals);
}

main();

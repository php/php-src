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

    $data = [];
    $data['Zend/bench.php'] = runBench();
    $data['Symfony Demo 2.2.3'] = runSymfonyDemo();
    $data['Wordpress 6.2'] = runWordpress();
    $result = json_encode($data, JSON_PRETTY_PRINT) . "\n";

    fwrite(STDOUT, $result);

    if ($storeResult) {
        storeResult($result);
    }
}

function storeResult(string $result) {
    $repo = __DIR__ . '/repos/data';
    cloneRepo($repo, 'git@github.com:iluuu1994/php-benchmark-data.git');

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

function runBench(): array {
    $process = runValgrindPhpCgiCommand([dirname(__DIR__) . '/Zend/bench.php']);
    return ['instructions' => extractInstructionsFromValgrindOutput($process->stderr)];
}

function runSymfonyDemo(): array {
    $dir = __DIR__ . '/repos/symfony-demo-2.2.3';
    cloneRepo($dir, 'https://github.com/iluuu1994/symfony-demo-2.2.3.git');
    runPhpCommand([$dir . '/bin/console', 'cache:clear']);
    runPhpCommand([$dir . '/bin/console', 'cache:warmup']);
    $process = runValgrindPhpCgiCommand(['-T1,1', $dir . '/public/index.php']);
    return ['instructions' => extractInstructionsFromValgrindOutput($process->stderr)];
}

function runWordpress(): array {
    $dir = __DIR__ . '/repos/wordpress-6.2';
    cloneRepo($dir, 'https://github.com/iluuu1994/wordpress-6.2.git');

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
    $process = runValgrindPhpCgiCommand(['-T1,1', $dir . '/index.php'], $dir);
    return ['instructions' => extractInstructionsFromValgrindOutput($process->stderr)];
}

function runPhpCommand(array $args, ?string $cwd = null): ProcessResult {
    return runCommand([PHP_BINARY, ...$args], $cwd);
}

function runValgrindPhpCgiCommand(array $args, ?string $cwd = null): ProcessResult {
    global $phpCgi;
    return runCommand([
        'valgrind',
        '--tool=callgrind',
        '--dump-instr=yes',
        '--callgrind-out-file=/dev/null',
        '--',
        $phpCgi,
        '-d max_execution_time=0',
        ...$args,
    ]);
}

function extractInstructionsFromValgrindOutput(string $output): ?string {
    preg_match("(==[0-9]+== Events    : Ir\n==[0-9]+== Collected : (?<instructions>[0-9]+))", $output, $matches);
    return $matches['instructions'] ?? null;
}

main();

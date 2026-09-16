--TEST--
CGI/FastCGI: a boundary segment file formatted in an earlier boot is reformatted instead of adopted
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip boot identity check is exercised on Linux only');
if (!is_readable('/proc/sys/kernel/random/boot_id')) die('skip /proc/sys/kernel/random/boot_id not readable');
if (!function_exists('proc_open')) die('skip proc_open() not available');
$root = dirname(__DIR__, 3);
$candidates = [
    getenv('TEST_PHP_CGI_EXECUTABLE') ?: null,
    $root . '/sapi/cgi/php-cgi',
];
foreach ($candidates as $candidate) {
    if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
        return;
    }
}
die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

function user_cache_cgi_binary(): string
{
    $root = dirname(__DIR__, 3);
    foreach ([getenv('TEST_PHP_CGI_EXECUTABLE') ?: null, $root . '/sapi/cgi/php-cgi'] as $candidate) {
        if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
            return $candidate;
        }
    }

    throw new RuntimeException('CGI SAPI binary not available');
}

function user_cache_cgi_run(string $phpCgi, string $script, string $docRoot, string $lockfilePath, string $query): array
{
    $process = proc_open(
        [
            $phpCgi, '-n', '-q',
            '-d', 'user_cache.enable=1',
            '-d', 'user_cache.shm_size=16M',
            '-d', 'user_cache.lockfile_path=' . $lockfilePath,
            '-d', 'display_errors=0',
            '-d', 'log_errors=0',
        ],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $docRoot,
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'SCRIPT_FILENAME' => $script,
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'stale-image.local',
            'QUERY_STRING' => $query,
        ]
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start php-cgi');
    }

    fclose($pipes[0]);
    $stdout = stream_get_contents($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);

    $lines = preg_split('/\r?\n/', trim($stdout));

    return [end($lines), trim($stderr)];
}

function user_cache_cgi_rm_rf(string $path): void
{
    if (!file_exists($path) && !is_link($path)) {
        return;
    }

    if (!is_dir($path) || is_link($path)) {
        unlink($path);
        return;
    }

    foreach (scandir($path) as $entry) {
        if ($entry === '.' || $entry === '..') {
            continue;
        }
        user_cache_cgi_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
    }

    rmdir($path);
}

/* The header records the first 64 bits of the kernel boot_id (dashes
 * stripped) as a native-endian uint64_t. Probe the byte order with a 32-bit
 * format code: 'J'/'P'/'Q' are unavailable on 32-bit builds. */
function user_cache_boot_token(): string
{
    $hex = substr(str_replace('-', '', trim(file_get_contents('/proc/sys/kernel/random/boot_id'))), 0, 16);
    $bigEndian = hex2bin($hex);

    return pack('L', 1) === pack('N', 1) ? $bigEndian : strrev($bigEndian);
}

function user_cache_boot_token_offset(string $segment, string $token): int|false
{
    $header = file_get_contents($segment, false, null, 0, 512);

    return strpos($header, $token);
}

$root = sys_get_temp_dir() . '/php-user-cache-cgi-stale-image-' . getmypid();
$docRoot = $root . '/doc';
$script = $docRoot . '/index.php';
$lockfilePath = $root . '/lock';

user_cache_cgi_rm_rf($root);
mkdir($docRoot, 0777, true);
mkdir($lockfilePath, 0777);

$privateDir = $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root);

file_put_contents($script, <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('stale-image');
if (($_GET['action'] ?? 'fetch') === 'seed') {
    $cache->store('key', 'seeded');
}
echo UserCache\Cache::getStatus()->getAvailability()->name, ':', $cache->fetch('key', 'MISS'), "\n";
PHP);

try {
    $phpCgi = user_cache_cgi_binary();
    $token = user_cache_boot_token();

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed');
    echo "seed: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=fetch');
    echo "same boot: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";

    $segments = glob($privateDir . '/*.seg');
    var_dump(count($segments));
    $segment = $segments[0];

    $offset = user_cache_boot_token_offset($segment, $token);
    var_dump($offset !== false);

    /* Forge an image left by another boot: same layout, different boot. */
    $handle = fopen($segment, 'r+b');
    fseek($handle, $offset);
    fwrite($handle, $token ^ str_repeat("\xff", strlen($token)));
    fclose($handle);
    var_dump(user_cache_boot_token_offset($segment, $token));

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=fetch');
    echo "other boot: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    var_dump(user_cache_boot_token_offset($segment, $token) === $offset);

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed');
    echo "reseed: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=fetch');
    echo "refetch: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";

    echo "Done\n";
} finally {
    user_cache_cgi_rm_rf($root);
}

?>
--EXPECT--
seed: Available:seeded
same boot: Available:seeded
int(1)
bool(true)
bool(false)
other boot: Available:MISS
bool(true)
reseed: Available:seeded
refetch: Available:seeded
Done

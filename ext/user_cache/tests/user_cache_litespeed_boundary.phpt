--TEST--
UserCache\Cache: litespeed partitions cache data by boundary
--EXTENSIONS--
user_cache
--CONFLICTS--
all
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
$root = dirname(__DIR__, 3);
$candidates = [
    getenv('TEST_PHP_LSAPI') ?: null,
    $root . '/sapi/litespeed/lsphp',
    $root . '/sapi/litespeed/php',
];
foreach ($candidates as $candidate) {
    if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
        return;
    }
}
die('skip litespeed SAPI binary not available; set TEST_PHP_LSAPI');
?>
--FILE--
<?php

const LSAPI_BEGIN_REQUEST = 1;
const LSAPI_RESP_STREAM = 4;
const LSAPI_RESP_END = 5;
const LSAPI_CONN_CLOSE = 8;
const LSAPI_REQ_HEADER_LEN = 44;
const LSAPI_HTTP_HEADER_INDEX_LEN = 152;

function user_cache_lsapi_binary(): string
{
    $root = dirname(__DIR__, 3);
    foreach ([getenv('TEST_PHP_LSAPI') ?: null, $root . '/sapi/litespeed/lsphp', $root . '/sapi/litespeed/php'] as $candidate) {
        if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
            return $candidate;
        }
    }

    throw new RuntimeException('litespeed SAPI binary not available');
}

function user_cache_lsapi_is_little_endian(): bool
{
    return pack('L', 1) === "\1\0\0\0";
}

function user_cache_lsapi_pack_int(int $value): string
{
    return pack(user_cache_lsapi_is_little_endian() ? 'V' : 'N', $value);
}

function user_cache_lsapi_unpack_int(string $value): int
{
    return unpack(user_cache_lsapi_is_little_endian() ? 'V' : 'N', $value)[1];
}

function user_cache_lsapi_packet_header(int $type, int $length): string
{
    $machine = php_uname('m');
    $flag = preg_match('/^(x86_64|i[3-6]86)$/i', $machine) ? 0 : 1;

    return 'LS' . chr($type) . chr($flag) . user_cache_lsapi_pack_int($length);
}

function user_cache_lsapi_env_block(array $env, int $baseOffset, array &$offsets): string
{
    $block = '';

    foreach ($env as $key => $value) {
        $keyString = $key . "\0";
        $valueString = $value . "\0";
        $offsets[$key] = $baseOffset + strlen($block) + 4 + strlen($keyString);
        $block .= pack('n', strlen($keyString));
        $block .= pack('n', strlen($valueString));
        $block .= $keyString;
        $block .= $valueString;
    }

    return $block . "\0\0\0\0";
}

function user_cache_lsapi_request_packet(string $script, string $docRoot, string $serverName, string $query): string
{
    $offsets = [];
    $specialBlock = "\0\0\0\0";
    $envBase = LSAPI_REQ_HEADER_LEN + strlen($specialBlock);
    $env = [
        'SCRIPT_FILENAME' => $script,
        'SCRIPT_NAME' => '/index.php',
        'QUERY_STRING' => $query,
        'REQUEST_METHOD' => 'GET',
        'SERVER_NAME' => $serverName,
        'DOCUMENT_ROOT' => $docRoot,
        'REQUEST_URI' => '/index.php' . ($query !== '' ? '?' . $query : ''),
        'SERVER_PROTOCOL' => 'HTTP/1.1',
        'REMOTE_ADDR' => '127.0.0.1',
    ];
    $envBlock = user_cache_lsapi_env_block($env, $envBase, $offsets);
    $body = $specialBlock . $envBlock;
    $body .= str_repeat("\0", (8 - ((LSAPI_REQ_HEADER_LEN + strlen($body)) % 8)) & 7);
    $body .= str_repeat("\0", LSAPI_HTTP_HEADER_INDEX_LEN);
    $length = LSAPI_REQ_HEADER_LEN + strlen($body);

    return user_cache_lsapi_packet_header(LSAPI_BEGIN_REQUEST, $length)
        . user_cache_lsapi_pack_int(0)
        . user_cache_lsapi_pack_int(0)
        . user_cache_lsapi_pack_int($offsets['SCRIPT_FILENAME'])
        . user_cache_lsapi_pack_int($offsets['SCRIPT_NAME'])
        . user_cache_lsapi_pack_int($offsets['QUERY_STRING'])
        . user_cache_lsapi_pack_int($offsets['REQUEST_METHOD'])
        . user_cache_lsapi_pack_int(0)
        . user_cache_lsapi_pack_int(count($env))
        . user_cache_lsapi_pack_int(0)
        . $body;
}

function user_cache_lsapi_read_exact($fp, int $length): string
{
    $buffer = '';

    while (strlen($buffer) < $length && !feof($fp)) {
        $chunk = fread($fp, $length - strlen($buffer));
        if ($chunk === false) {
            throw new RuntimeException('Failed to read LSAPI response');
        }
        if ($chunk === '') {
            if (stream_get_meta_data($fp)['timed_out']) {
                throw new RuntimeException('Timed out reading LSAPI response');
            }
            usleep(10000);
            continue;
        }
        $buffer .= $chunk;
    }

    if (strlen($buffer) !== $length) {
        throw new RuntimeException('Truncated LSAPI response');
    }

    return $buffer;
}

function user_cache_lsapi_request(int $port, string $script, string $docRoot, string $serverName, string $query): string
{
    $fp = @stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 2);
    if ($fp === false) {
        throw new RuntimeException($errstr);
    }
    stream_set_timeout($fp, 5);

    fwrite($fp, user_cache_lsapi_request_packet($script, $docRoot, $serverName, $query));
    $body = '';

    while (!feof($fp)) {
        try {
            $header = user_cache_lsapi_read_exact($fp, 8);
        } catch (RuntimeException $e) {
            if ($body !== '' && $e->getMessage() === 'Truncated LSAPI response') {
                break;
            }
            throw $e;
        }
        if (substr($header, 0, 2) !== 'LS') {
            throw new RuntimeException('Invalid LSAPI response header');
        }

        $type = ord($header[2]);
        $length = user_cache_lsapi_unpack_int(substr($header, 4, 4));
        $payload = $length > 8 ? user_cache_lsapi_read_exact($fp, $length - 8) : '';

        if ($type === LSAPI_RESP_STREAM) {
            $body .= $payload;
        } elseif ($type === LSAPI_RESP_END || $type === LSAPI_CONN_CLOSE) {
            break;
        }
    }

    fclose($fp);

    return trim($body);
}

function user_cache_lsapi_free_port(): int
{
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    if ($server === false) {
        throw new RuntimeException($errstr);
    }

    $name = stream_socket_get_name($server, false);
    fclose($server);

    return (int) substr(strrchr($name, ':'), 1);
}

function user_cache_lsapi_rm_rf(string $path): void
{
    if (!file_exists($path)) {
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
        user_cache_lsapi_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
    }

    rmdir($path);
}

function user_cache_lsapi_start(string $lsphp, string $iniDir, int $port): array
{
    $env = array_merge($_ENV, [
        'PHP_LSAPI_CHILDREN' => '1',
        'LSAPI_PPID_NO_CHECK' => '1',
        'LSAPI_MAX_REQS' => '100',
        'LSAPI_CLEAN_SHUTDOWN' => '1',
    ]);
    $process = proc_open(
        [$lsphp, '-c', $iniDir, '-b', "127.0.0.1:$port"],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $iniDir,
        $env
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start lsphp');
    }
    stream_set_blocking($pipes[1], false);
    stream_set_blocking($pipes[2], false);

    return [$process, $pipes];
}

function user_cache_lsapi_stop($process, array $pipes): void
{
    if (is_resource($process)) {
        proc_terminate($process);

        for ($i = 0; $i < 20; $i++) {
            $status = proc_get_status($process);
            if (!$status['running']) {
                break;
            }
            usleep(100000);
        }

        $status = proc_get_status($process);
        if ($status['running']) {
            proc_terminate($process, 9);
        }
    }

    foreach ($pipes as $pipe) {
        if (is_resource($pipe)) {
            fclose($pipe);
        }
    }

    if (is_resource($process)) {
        proc_close($process);
    }
}

function user_cache_lsapi_wait($process, array $pipes, int $port, string $script, string $docRoot, string $serverName): void
{
    for ($i = 0; $i < 50; $i++) {
        $status = proc_get_status($process);
        if (!$status['running']) {
            throw new RuntimeException(stream_get_contents($pipes[2]));
        }

        try {
            user_cache_lsapi_request($port, $script, $docRoot, $serverName, 'action=fetch');
            return;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    throw new RuntimeException(stream_get_contents($pipes[2]) ?: 'lsphp did not become ready');
}

$root = sys_get_temp_dir() . '/php-user-cache-lsapi-' . getmypid();
$alphaRoot = $root . '/alpha';
$betaRoot = $root . '/beta';
$otherRoot = $root . '/other';
$processes = [];

user_cache_lsapi_rm_rf($root);
mkdir($alphaRoot, 0777, true);
mkdir($betaRoot, 0777, true);
mkdir($otherRoot, 0777, true);

$script = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('default');
$key = 'litespeed-boundary-key';
$action = $_GET['action'] ?? 'fetch';
$host = $_SERVER['SERVER_NAME'] ?? 'unknown';

if ($action === 'seed') {
    $cache->store($key, $host . '-value');
}

echo $host, ':', $cache->fetch($key, 'MISS'), ':', $cache->getPoolStatus()->getPoolName(), "\n";
PHP;

file_put_contents($alphaRoot . '/index.php', $script);
file_put_contents($betaRoot . '/index.php', $script);
file_put_contents($otherRoot . '/index.php', $script);
file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=32M',
    'opcache.file_update_protection=0',
]));

try {
    $lsphp = user_cache_lsapi_binary();
    $alphaPort = user_cache_lsapi_free_port();
    $betaPort = user_cache_lsapi_free_port();
    $alphaScript = $alphaRoot . '/index.php';
    $betaScript = $betaRoot . '/index.php';
    $otherScript = $otherRoot . '/index.php';

    $processes[] = user_cache_lsapi_start($lsphp, $root, $alphaPort);
    $processes[] = user_cache_lsapi_start($lsphp, $root, $betaPort);

    user_cache_lsapi_wait($processes[0][0], $processes[0][1], $alphaPort, $alphaScript, $alphaRoot, 'alpha.local');
    user_cache_lsapi_wait($processes[1][0], $processes[1][1], $betaPort, $betaScript, $betaRoot, 'beta.local');

    $checks = [
        [$alphaPort, $alphaScript, $alphaRoot, 'alpha.local', 'action=seed', 'alpha.local:alpha.local-value:default'],
        [$alphaPort, $alphaScript, $alphaRoot, 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:default'],
        [$alphaPort, $otherScript, $otherRoot, 'other.local', 'action=fetch', 'other.local:MISS:default'],
        [$alphaPort, $otherScript, $otherRoot, 'other.local', 'action=seed', 'other.local:other.local-value:default'],
        [$alphaPort, $otherScript, $otherRoot, 'other.local', 'action=fetch', 'other.local:other.local-value:default'],
        [$alphaPort, $alphaScript, $alphaRoot, 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:default'],
        [$betaPort, $betaScript, $betaRoot, 'beta.local', 'action=fetch', 'beta.local:MISS:default'],
        [$betaPort, $betaScript, $betaRoot, 'beta.local', 'action=seed', 'beta.local:beta.local-value:default'],
        [$alphaPort, $alphaScript, $alphaRoot, 'alpha.local', 'action=fetch', 'alpha.local:alpha.local-value:default'],
        [$betaPort, $betaScript, $betaRoot, 'beta.local', 'action=fetch', 'beta.local:beta.local-value:default'],
    ];

    foreach ($checks as [$port, $file, $docRoot, $serverName, $query, $expected]) {
        $actual = user_cache_lsapi_request($port, $file, $docRoot, $serverName, $query);
        if ($actual !== $expected) {
            $stderr = stream_get_contents($port === $alphaPort ? $processes[0][1][2] : $processes[1][1][2]);
            throw new RuntimeException("Expected $expected, got $actual" . ($stderr !== '' ? "\n$stderr" : ''));
        }
    }

    echo "Done\n";
} finally {
    foreach ($processes as [$process, $pipes]) {
        user_cache_lsapi_stop($process, $pipes);
    }
    user_cache_lsapi_rm_rf($root);
}

?>
--EXPECT--
Done

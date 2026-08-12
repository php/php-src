--TEST--
CGI/FastCGI: UserCache disables new boundary partitions and logs after the limit
--CONFLICTS--
all
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI boundary server test is not supported on Windows');
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

const FCGI_BEGIN_REQUEST = 1;
const FCGI_END_REQUEST = 3;
const FCGI_PARAMS = 4;
const FCGI_STDIN = 5;
const FCGI_STDOUT = 6;
const FCGI_STDERR = 7;
const FCGI_RESPONDER = 1;

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

function user_cache_cgi_free_port(): int
{
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    if ($server === false) {
        throw new RuntimeException($errstr);
    }

    $name = stream_socket_get_name($server, false);
    fclose($server);

    return (int) substr(strrchr($name, ':'), 1);
}

function user_cache_cgi_record(int $type, string $content, int $requestId = 1): string
{
    $padding = (8 - (strlen($content) % 8)) % 8;

    return chr(1)
        . chr($type)
        . pack('n', $requestId)
        . pack('n', strlen($content))
        . chr($padding)
        . "\0"
        . $content
        . str_repeat("\0", $padding);
}

function user_cache_cgi_name_value(string $name, string $value): string
{
    $nameLen = strlen($name);
    $valueLen = strlen($value);

    return ($nameLen < 128 ? chr($nameLen) : pack('N', $nameLen | 0x80000000))
        . ($valueLen < 128 ? chr($valueLen) : pack('N', $valueLen | 0x80000000))
        . $name
        . $value;
}

function user_cache_cgi_params(array $params): string
{
    $body = '';
    foreach ($params as $name => $value) {
        $body .= user_cache_cgi_name_value($name, $value);
    }

    return $body;
}

function user_cache_cgi_read_exact($fp, int $length): string
{
    $buffer = '';
    while (strlen($buffer) < $length && !feof($fp)) {
        $chunk = fread($fp, $length - strlen($buffer));
        if ($chunk === false) {
            throw new RuntimeException('Failed to read FastCGI response');
        }
        if ($chunk === '') {
            if (stream_get_meta_data($fp)['timed_out']) {
                throw new RuntimeException('Timed out reading FastCGI response');
            }
            usleep(10000);
            continue;
        }
        $buffer .= $chunk;
    }

    if (strlen($buffer) !== $length) {
        throw new RuntimeException('Truncated FastCGI response');
    }

    return $buffer;
}

function user_cache_cgi_request(int $port, string $script, string $docRoot, string $host, string $query): string
{
    $fp = @stream_socket_client("tcp://127.0.0.1:$port", $errno, $errstr, 2);
    if ($fp === false) {
        throw new RuntimeException($errstr);
    }
    stream_set_timeout($fp, 5);

    $params = [
        'SCRIPT_FILENAME' => $script,
        'SCRIPT_NAME' => '/index.php',
        'QUERY_STRING' => $query,
        'REQUEST_METHOD' => 'GET',
        'SERVER_NAME' => $host,
        'HTTP_HOST' => $host,
        'DOCUMENT_ROOT' => $docRoot,
        'REQUEST_URI' => '/index.php' . ($query !== '' ? '?' . $query : ''),
        'SERVER_PROTOCOL' => 'HTTP/1.1',
        'REMOTE_ADDR' => '127.0.0.1',
        'REDIRECT_STATUS' => '1',
    ];

    fwrite($fp, user_cache_cgi_record(FCGI_BEGIN_REQUEST, pack('nC6', FCGI_RESPONDER, 0, 0, 0, 0, 0, 0)));
    fwrite($fp, user_cache_cgi_record(FCGI_PARAMS, user_cache_cgi_params($params)));
    fwrite($fp, user_cache_cgi_record(FCGI_PARAMS, ''));
    fwrite($fp, user_cache_cgi_record(FCGI_STDIN, ''));

    $stdout = '';
    $stderr = '';
    while (!feof($fp)) {
        $header = user_cache_cgi_read_exact($fp, 8);
        $type = ord($header[1]);
        $contentLength = unpack('n', substr($header, 4, 2))[1];
        $paddingLength = ord($header[6]);
        $content = $contentLength > 0 ? user_cache_cgi_read_exact($fp, $contentLength) : '';
        if ($paddingLength > 0) {
            user_cache_cgi_read_exact($fp, $paddingLength);
        }

        if ($type === FCGI_STDOUT) {
            $stdout .= $content;
        } elseif ($type === FCGI_STDERR) {
            $stderr .= $content;
        } elseif ($type === FCGI_END_REQUEST) {
            break;
        }
    }
    fclose($fp);

    if ($stderr !== '') {
        throw new RuntimeException($stderr);
    }

    $parts = preg_split("/\r?\n\r?\n/", $stdout, 2);

    return trim($parts[1] ?? $stdout);
}

function user_cache_cgi_rm_rf(string $path): void
{
    if (!file_exists($path)) {
        return;
    }
    if (!is_dir($path) || is_link($path)) {
        unlink($path);
        return;
    }
    foreach (scandir($path) as $entry) {
        if ($entry !== '.' && $entry !== '..') {
            user_cache_cgi_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
        }
    }
    rmdir($path);
}

$root = sys_get_temp_dir() . '/php-user-cache-cgi-boundary-limit-' . getmypid();
$docRoot = $root . '/site';
$log = $root . '/error.log';
$process = null;
$pipes = [];

user_cache_cgi_rm_rf($root);
mkdir($docRoot, 0777, true);
file_put_contents($docRoot . '/index.php', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('default');
$action = $_GET['action'] ?? 'fetch';
$host = $_SERVER['SERVER_NAME'] ?? 'unknown';
if ($action === 'seed') {
    $cache->store('key', $host . '-value');
}
$availability = UserCache\Cache::getStatus()->getAvailability();
echo $host, ':', $cache->fetch('key', 'MISS'), ':', $availability->name, "\n";
PHP);
file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=1M',
    'opcache.file_update_protection=0',
    'display_errors=0',
    'log_errors=1',
    'error_log=' . $log,
]));

try {
    $port = user_cache_cgi_free_port();
    $script = $docRoot . '/index.php';
    $process = proc_open(
        [user_cache_cgi_binary(), '-c', $root, '-b', "127.0.0.1:$port"],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $root
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start php-cgi');
    }
    stream_set_blocking($pipes[1], false);
    stream_set_blocking($pipes[2], false);

    for ($i = 0; $i < 50; $i++) {
        try {
            user_cache_cgi_request($port, $script, $docRoot, 'host01.local', 'action=fetch');
            break;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    $expected = 'host01.local:host01.local-value:Available';
    $actual = user_cache_cgi_request($port, $script, $docRoot, 'host01.local', 'action=seed');
    if ($actual !== $expected) {
        throw new RuntimeException("Expected $expected, got $actual");
    }

    for ($i = 2; $i <= 32; $i++) {
        $host = sprintf('host%02d.local', $i);
        $expected = $host . ':MISS:Available';
        $actual = user_cache_cgi_request($port, $script, $docRoot, $host, 'action=fetch');
        if ($actual !== $expected) {
            throw new RuntimeException("Expected $expected, got $actual");
        }
    }

    foreach (['host33.local', 'host34.local'] as $host) {
        $expected = $host . ':MISS:UnavailableByCgiFastCgiBoundary';
        $actual = user_cache_cgi_request($port, $script, $docRoot, $host, 'action=seed');
        if ($actual !== $expected) {
            throw new RuntimeException("Expected $expected, got $actual");
        }
    }

    $expected = 'host01.local:host01.local-value:Available';
    $actual = user_cache_cgi_request($port, $script, $docRoot, 'host01.local', 'action=fetch');
    if ($actual !== $expected) {
        throw new RuntimeException("Expected $expected, got $actual");
    }

    $contents = is_file($log) ? file_get_contents($log) : '';
    $needle = 'UserCache boundary partition limit (32) reached; creation of new partitions has been disabled';
    if (substr_count($contents, $needle) !== 1) {
        throw new RuntimeException("Expected one boundary-limit error-log entry, got:\n" . $contents);
    }

    echo "Done\n";
} finally {
    if (is_resource($process)) {
        proc_terminate($process);
        proc_close($process);
    }
    user_cache_cgi_rm_rf($root);
}
?>
--EXPECT--
Done

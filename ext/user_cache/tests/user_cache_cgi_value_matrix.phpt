--TEST--
CGI/FastCGI: UserCache\Cache round-trips a rich value matrix across the cross-process named segment
--EXTENSIONS--
user_cache
--CONFLICTS--
all
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI value matrix server test is not supported on Windows');
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
    $encoded = '';

    $encoded .= $nameLen < 128 ? chr($nameLen) : pack('N', $nameLen | 0x80000000);
    $encoded .= $valueLen < 128 ? chr($valueLen) : pack('N', $valueLen | 0x80000000);

    return $encoded . $name . $value;
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

function user_cache_cgi_request(int $port, string $script, string $docRoot, string $serverName, string $query): string
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
        'SERVER_NAME' => $serverName,
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

function user_cache_cgi_wait($process, array $pipes, int $port, string $script, string $docRoot, string $serverName): void
{
    for ($i = 0; $i < 50; $i++) {
        $status = proc_get_status($process);
        if (!$status['running']) {
            throw new RuntimeException(stream_get_contents($pipes[2]));
        }

        try {
            user_cache_cgi_request($port, $script, $docRoot, $serverName, 'action=warmup');
            return;
        } catch (Throwable) {
            usleep(100000);
        }
    }

    throw new RuntimeException(stream_get_contents($pipes[2]) ?: 'php-cgi did not become ready');
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
        if ($entry === '.' || $entry === '..') {
            continue;
        }
        user_cache_cgi_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
    }

    rmdir($path);
}

$root = sys_get_temp_dir() . '/php-user-cache-cgi-matrix-' . getmypid();
$docRoot = $root . '/site';
$process = null;
$pipes = [];

user_cache_cgi_rm_rf($root);
mkdir($docRoot, 0777, true);

/* Compare values across processes through native serialization. */
$script = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('default');
$action = $_GET['action'] ?? 'verify';

enum MatrixSuit: string
{
    case Hearts = 'h';
    case Spades = 's';
}

class MatrixPlain
{
    public int $pub = 1;
    protected string $prot = 'p';
    private array $priv = ['x'];
}

class MatrixMagic
{
    public function __construct(public array $state)
    {
    }

    public function __serialize(): array
    {
        return ['state' => $this->state];
    }

    public function __unserialize(array $data): void
    {
        $this->state = $data['state'];
    }
}

class MatrixSleep
{
    public int $kept = 5;
    public string $dropped = 'gone';

    public function __sleep(): array
    {
        return ['kept'];
    }

    public function __wakeup(): void
    {
    }
}

class MatrixLegacy implements Serializable
{
    public function __construct(public int $v = 0)
    {
    }

    public function serialize(): string
    {
        return (string) $this->v;
    }

    public function unserialize(string $data): void
    {
        $this->v = (int) $data;
    }
}

function matrix_build(): array
{
    $shared = new stdClass();
    $shared->tag = 'shared';

    $cycleA = new stdClass();
    $cycleB = new stdClass();
    $cycleA->peer = $cycleB;
    $cycleB->peer = $cycleA;

    $ref = 1;
    $withRef = ['a' => &$ref, 'b' => &$ref];

    $storage = new SplObjectStorage();
    $storageKey = new stdClass();
    $storageKey->id = 1;
    $storage[$storageKey] = ['data' => [1, 2, 3]];

    $stack = new SplStack();
    $stack->push('a');
    $stack->push('b');

    return [
        'scalars' => [null, true, PHP_INT_MIN, PHP_INT_MAX, 1.5, 'str', "bin\0\xff\xfe"],
        'arrays' => ['packed' => [1, 2, 3], 'hashed' => ['a' => 1, -5 => 'neg'], 'nested' => ['x' => ['y' => [1, 2]]]],
        'refs' => $withRef,
        'cycle' => $cycleA,
        'plain' => new MatrixPlain(),
        'enum' => MatrixSuit::Spades,
        'magic' => new MatrixMagic(['id' => 7, 'items' => [1, 2, 3]]),
        'sleep' => new MatrixSleep(),
        'legacy' => new MatrixLegacy(99),
        'date' => new DateTimeImmutable('2026-07-09 12:34:56.123456', new DateTimeZone('Asia/Tokyo')),
        'interval' => new DateInterval('P1Y2DT3H'),
        'period' => new DatePeriod(new DateTimeImmutable('2026-01-01'), new DateInterval('P1D'), 2),
        'stack' => $stack,
        'arrayobject' => new ArrayObject(['k' => 'v', 'n' => [1, 2]]),
        'storage' => $storage,
        'sharedid' => [$shared, $shared],
    ];
}

if ($action === 'seed') {
    foreach (matrix_build() as $key => $value) {
        if (!$cache->store('m_' . $key, $value)) {
            echo 'seed-fail:', $key, "\n";
            exit;
        }
    }
    echo "seeded:ok\n";
    exit;
}

if ($action === 'warmup') {
    echo "warmup:ok\n";
    exit;
}

$mismatch = [];
foreach (matrix_build() as $key => $expected) {
    $missing = new stdClass();
    $fetched = $cache->fetch('m_' . $key, $missing);
    if ($fetched === $missing) {
        $mismatch[] = $key . ':MISS';
        continue;
    }
    if (serialize($fetched) !== serialize($expected)) {
        $mismatch[] = $key . ':DIFF';
    }
}

echo $mismatch === [] ? "verify:ok\n" : ('verify:FAIL ' . implode(',', $mismatch) . "\n");
PHP;

file_put_contents($docRoot . '/index.php', $script);
file_put_contents($root . '/php.ini', implode("\n", [
    'user_cache.enable=1',
    'user_cache.shm_size=32M',
    'opcache.file_update_protection=0',
    'date.timezone=UTC',
    'error_reporting=E_ALL & ~E_DEPRECATED',
    'display_errors=0',
]));

try {
    $phpCgi = user_cache_cgi_binary();
    $port = user_cache_cgi_free_port();
    $scriptFile = $docRoot . '/index.php';

    $process = proc_open(
        [$phpCgi, '-c', $root, '-b', "127.0.0.1:$port"],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $root
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start php-cgi');
    }
    stream_set_blocking($pipes[1], false);
    stream_set_blocking($pipes[2], false);

    user_cache_cgi_wait($process, $pipes, $port, $scriptFile, $docRoot, 'matrix.local');

    $checks = [
        [$scriptFile, $docRoot, 'matrix.local', 'action=seed', 'seeded:ok'],
        [$scriptFile, $docRoot, 'matrix.local', 'action=verify', 'verify:ok'],
    ];

    foreach ($checks as [$file, $root2, $serverName, $query, $expected]) {
        $actual = user_cache_cgi_request($port, $file, $root2, $serverName, $query);
        if ($actual !== $expected) {
            $stderr = stream_get_contents($pipes[2]);
            throw new RuntimeException("Expected $expected, got $actual" . ($stderr !== '' ? "\n$stderr" : ''));
        }
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

--TEST--
UserCache\Cache: FastCGI round-trips a rich value matrix across the cross-process named segment
--CONFLICTS--
all
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip FastCGI value matrix server test is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$server = new FcgiTester('value-matrix');

try {
    /* Seed a rich value matrix in one request and compare it through serialize() in the next */
    $docRoot = $server->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('value-matrix');
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
PHP);
    $script = $docRoot . '/index.php';

    $server->start([
        'user_cache.enable=1',
        'user_cache.shm_size=32M',
        'opcache.file_update_protection=0',
        'date.timezone=UTC',
        'error_reporting=E_ALL & ~E_DEPRECATED',
        'display_errors=0',
    ], $script, $docRoot, 'matrix.local', 'action=warmup');

    $checks = [
        ['action=seed', 'seeded:ok'],
        ['action=verify', 'verify:ok'],
    ];

    foreach ($checks as [$query, $expected]) {
        $server->expect($expected, $script, $docRoot, 'matrix.local', $query);
    }

    echo "Done\n";
} finally {
    $server->cleanup();
}

?>
--EXPECT--
Done

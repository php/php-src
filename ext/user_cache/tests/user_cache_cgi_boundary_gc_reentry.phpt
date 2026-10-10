--TEST--
UserCache\Cache: destructors run by the cycle collector inside CGI boundary requests may re-create a deleted pool, refill a key record whose value is being released, see an empty pool registry at request shutdown and see consistent records while records are dropped
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
$root = dirname(__DIR__, 3);
foreach ([getenv('TEST_PHP_CGI_EXECUTABLE') ?: null, $root . '/sapi/cgi/php-cgi'] as $candidate) {
    if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
        return;
    }
}
die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

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

function user_cache_cgi_boundary_run(string $name, string $code, array $ini = []): void
{
    $root = sys_get_temp_dir() . '/php-user-cache-cgi-' . $name . '-' . getmypid();
    $docRoot = $root . '/doc';
    $lockfilePath = $root . '/lock';
    $script = $docRoot . '/index.php';
    @mkdir($docRoot, 0777, true);
    @mkdir($lockfilePath, 0777, true);

    file_put_contents($script, $code);

    try {
        $process = proc_open(
            [
                user_cache_cgi_binary(), '-n', '-q',
                '-d', 'user_cache.enable=1',
                '-d', 'user_cache.shm_size=16M',
                '-d', 'user_cache.lockfile_path=' . $lockfilePath,
                '-d', 'display_errors=1',
                ...$ini,
            ],
            [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
            $pipes,
            $docRoot,
            [
                'REDIRECT_STATUS' => '1',
                'REQUEST_METHOD' => 'GET',
                'SCRIPT_FILENAME' => $script,
                'DOCUMENT_ROOT' => $docRoot,
                'SERVER_NAME' => $name . '.local',
                'USE_ZEND_ALLOC' => getenv('USE_ZEND_ALLOC') ?: '1',
            ]
        );
        fclose($pipes[0]);
        /* A startup notice flushes the CGI headers before -q applies. */
        echo trim(preg_replace('/\A(?:[A-Za-z-]+: [^\r\n]*\r?\n)+\r?\n/', '', stream_get_contents($pipes[1]))), "\n";
        $stderr = trim(stream_get_contents($pipes[2]));
        fclose($pipes[1]);
        fclose($pipes[2]);
        echo 'exit: ', proc_close($process), $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    } finally {
        FcgiTester::removeBoundarySegments($root);
        foreach ([$script, $docRoot, $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root)] as $path) {
            if (is_dir($path)) {
                array_map('unlink', glob($path . '/{,.}[!.]*', GLOB_BRACE) ?: []);
                @rmdir($path);
            } elseif (file_exists($path)) {
                unlink($path);
            }
        }
        @rmdir($lockfilePath);
        @rmdir($root);
    }
}

echo "delete pool destructor:\n";
/* Boundary partitions keep fetched arrays as private copies in the key records,
 * so deletePool() releases collectable values. */
user_cache_cgi_boundary_run('delete-pool', <<<'PHP'
<?php
use UserCache\Cache;

final class Probe
{
    public $self;

    public function __destruct()
    {
        global $replacement;

        Cache::deletePool('delete-pool');
        $replacement = Cache::getPool('delete-pool');
        $replacement->store('from-destructor', 1);
        echo "destructor re-created the pool\n";
    }
}

$pool = Cache::getPool('delete-pool');
$arrays = 3000;
for ($i = 0; $i < $arrays; $i++) {
    $pool->store("k$i", [$i, $i + 1, $i + 2]);
}
$held = [];
for ($i = 0; $i < $arrays; $i++) {
    $held[] = $pool->fetch("k$i");
}
gc_collect_cycles();
/* Fill the root buffer so that releasing the record values starts a collection. */
$keep = [];
for ($i = 0; $i < 8990; $i++) {
    $object = new stdClass;
    $keep[] = $object;
}
unset($object);
$probe = new Probe;
$probe->self = $probe;
unset($probe, $pool);
$replacement = null;
var_dump(Cache::deletePool('delete-pool'));
var_dump($replacement instanceof Cache);
var_dump(Cache::getPool('delete-pool') === $replacement);
var_dump(Cache::getPool('delete-pool')->fetch('from-destructor'));
PHP);

echo "\nrecord value reentry:\n";
/* Boundary partitions keep fetched arrays as collectable private copies in the key records, so releasing them can start
 * a collection. */
user_cache_cgi_boundary_run('record-reentry', <<<'PHP'
<?php
use UserCache\Cache;

final class Probe
{
    public $self;

    public function __construct(private Closure $action)
    {
    }

    public function __destruct()
    {
        ($this->action)();
    }
}

function arm(Closure $action): void
{
    gc_collect_cycles();
    $probe = new Probe($action);
    $probe->self = $probe;
    unset($probe);
    $GLOBALS['keep'] = [];
    while (gc_status()['roots'] < gc_status()['threshold'] - 2) {
        $object = new stdClass;
        $GLOBALS['keep'][] = $object;
    }
}

/* Two pool objects share one scope, so a destructor can put a new private copy into the record of the other one. */
$a = Cache::getPool('record-reentry');
Cache::deletePool('record-reentry');
$b = Cache::getPool('record-reentry');

$b->store('z', 1);
$b->store('z', 2);
$b->store('k', [1, 2, 3]);
$held = $a->fetch('k');
arm(function () use ($a, $b): void {
    echo "destructor before the scalar write\n";
    $b->store('k', [4, 5, 6]);
    $GLOBALS['held2'] = $a->fetch('k');
    $b->store('k', 5);
});
var_dump($a->store('k', 7), $a->fetch('k'));

$b->store('n', [1, 2, 3]);
$held3 = $a->fetch('n');
$b->store('n', 10);
arm(function () use ($a, $b): void {
    echo "destructor during the increment\n";
    $b->store('n', str_repeat('x', 300));
    var_dump(strlen($a->fetch('n')));
});
var_dump($a->increment('n', 1, 60), $a->fetch('n') === str_repeat('x', 300));
echo "done\n";
PHP, ['-d', 'report_memleaks=1']);

echo "\nshutdown pool registry:\n";
/* Boundary partitions keep fetched arrays as collectable private copies in the key records, so releasing the pools at
 * request shutdown can start a collection. */
user_cache_cgi_boundary_run('shutdown-pools', <<<'PHP'
<?php
use UserCache\Cache;

final class Probe
{
    public $self;

    public function __destruct()
    {
        echo "destructor at request shutdown\n";
        var_dump(Cache::hasPool('shutdown-pools'), Cache::deletePool('shutdown-pools'), Cache::getPools());
        var_dump(Cache::getPool('shutdown-pools')->fetch('k0'));
    }
}

$held = [];
$pool = Cache::getPool('shutdown-pools');
for ($i = 0; $i < 300; $i++) {
    $pool->store("k$i", [$i, 1, 2]);
    $held[] = $pool->fetch("k$i");
}
unset($pool);

/* Output callbacks run after the destructor phase, so only the cycle collector destroys what they create. */
ob_start(function (string $buffer): string {
    gc_collect_cycles();
    $probe = new Probe;
    $probe->self = $probe;
    unset($probe);
    $keep = [];
    while (gc_status()['roots'] < gc_status()['threshold'] - 100) {
        $object = new stdClass;
        $keep[] = $object;
    }
    $GLOBALS['keep'] = $keep;

    return $buffer . "output callback done\n";
});
echo "main done\n";
PHP);

echo "\nrecord trim destructor:\n";
/* Boundary partitions keep fetched arrays as private copies in the key records,
 * so dropping records releases collectable values. */
user_cache_cgi_boundary_run('record-trim', <<<'PHP'
<?php
use UserCache\Cache;

final class Probe
{
    public $self;

    public function __destruct()
    {
        global $pool, $dropping;

        echo 'destructor while dropping records: ', var_export($dropping, true), "\n";
        echo 'dropped key still readable: ', var_export($pool->fetch('k0') === [0, 1, 2], true), "\n";
    }
}

$pool = Cache::getPool('record-trim');
$limit = max(16384, Cache::getStatus()->getEntryCapacity());
$arrays = 3000;
for ($i = 0; $i < $arrays; $i++) {
    $pool->store("k$i", [$i, $i + 1, $i + 2]);
}
$held = [];
for ($i = 0; $i < $arrays; $i++) {
    $held[] = $pool->fetch("k$i");
}
for ($i = 0; $i < $limit - $arrays; $i++) {
    $pool->has("miss-$i");
}
gc_collect_cycles();
/* Fill the root buffer so that releasing the dropped values starts a collection. */
$keep = [];
for ($i = 0; $i < 8990; $i++) {
    $object = new stdClass;
    $keep[] = $object;
}
unset($object);
$probe = new Probe;
$probe->self = $probe;
unset($probe);
$dropping = true;
$pool->has('trigger');
$dropping = false;
echo "done\n";
PHP);
?>
--EXPECT--
delete pool destructor:
destructor re-created the pool
bool(true)
bool(true)
bool(true)
int(1)
exit: 0

record value reentry:
destructor before the scalar write
bool(true)
int(7)
destructor during the increment
int(300)
int(11)
bool(true)
done
exit: 0

shutdown pool registry:
main done
output callback done
destructor at request shutdown
bool(false)
bool(true)
array(0) {
}
NULL
exit: 0

record trim destructor:
destructor while dropping records: true
dropped key still readable: true
done
exit: 0

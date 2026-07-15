--TEST--
FPM: UserCache\Cache drops silently-corrupt entries but propagates decode exceptions
--EXTENSIONS--
user_cache
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[opcache]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('decode-failure');
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
    class UserCacheGone
    {
        public string $value = 'stored';
    }

    class UserCacheAutoloadThrows
    {
        public string $value = 'stored';
    }

    $cache->clear();
    var_dump($cache->store('single', new UserCacheGone()));
    var_dump($cache->store('multiple', new UserCacheGone()));
    var_dump($cache->store('autoload-single', new UserCacheAutoloadThrows()));
    var_dump($cache->store('autoload-multiple', new UserCacheAutoloadThrows()));
    var_dump($cache->has('single'));
    var_dump($cache->has('multiple'));
    var_dump($cache->has('autoload-single'));
    var_dump($cache->has('autoload-multiple'));
    return;
}

if ($action === 'fetch') {
    spl_autoload_register(function (string $class): void {
        if ($class === 'UserCacheAutoloadThrows') {
            throw new Exception('autoload failed');
        }
    });

    /* A missing class with no autoloader hit fails to decode without an
     * exception, so the entry is corrupt and dropped. */
    var_dump($cache->fetch('single', 'DEFAULT'));
    var_dump($cache->has('single'));

    /* A throwing autoloader is a genuine userland exception: it propagates and
     * the entry is kept, matching native unserialize(). */
    try {
        $cache->fetch('autoload-single', 'DEFAULT');
        echo "no exception\n";
    } catch (Exception $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
    var_dump($cache->has('autoload-single'));

    /* In a batch, the corrupt entry is dropped and the miss yields the default
     * before the throwing autoloader aborts the whole fetch. */
    try {
        $cache->fetchMultiple(['multiple', 'missing', 'autoload-multiple'], 'DEFAULT');
        echo "no exception\n";
    } catch (Exception $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
    var_dump($cache->has('multiple'));
    var_dump($cache->has('autoload-multiple'));
    return;
}
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)"
);

$tester->request(query: 'action=fetch')->expectBody(
    "string(7) \"DEFAULT\"\n" .
    "bool(false)\n" .
    "caught: autoload failed\n" .
    "bool(true)\n" .
    "caught: autoload failed\n" .
    "bool(false)\n" .
    "bool(true)"
);

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
unset($tester);
gc_collect_cycles();

echo "Done\n";

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

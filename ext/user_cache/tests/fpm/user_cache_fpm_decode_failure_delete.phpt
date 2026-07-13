--TEST--
FPM: UserCache\Cache deletes entries that fail to decode
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

    var_dump($cache->fetch('single', 'DEFAULT'));
    var_dump($cache->has('single'));
    var_dump($cache->fetch('autoload-single', 'DEFAULT'));
    var_dump($cache->has('autoload-single'));
    var_dump($cache->fetchMultiple(['multiple', 'missing', 'autoload-multiple'], 'DEFAULT'));
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
    "string(7) \"DEFAULT\"\n" .
    "bool(false)\n" .
    "array(3) {\n" .
    "  [\"multiple\"]=>\n" .
    "  string(7) \"DEFAULT\"\n" .
    "  [\"missing\"]=>\n" .
    "  string(7) \"DEFAULT\"\n" .
    "  [\"autoload-multiple\"]=>\n" .
    "  string(7) \"DEFAULT\"\n" .
    "}\n" .
    "bool(false)\n" .
    "bool(false)"
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

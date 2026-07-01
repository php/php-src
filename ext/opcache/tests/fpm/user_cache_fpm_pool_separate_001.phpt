--TEST--
FPM: OPcache User Cache is separated between pools
--EXTENSIONS--
opcache
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[alpha]
listen = {{ADDR[alpha]}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
[beta]
listen = {{ADDR[beta]}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
$action = $_GET['action'] ?? 'fetch';
$pool = $_GET['pool'] ?? 'unknown';
$cache = new Opcache\UserCache('default');
$key = 'fpm_pool_separate_shared_key';

if ($action === 'seed') {
    $cache->store($key, $pool . '-value');
}

printf(
    "%s:%s:%s\n",
    $pool,
    $cache->fetch($key, 'MISS'),
    $cache->info()->scope
);
PHP;

function expectPoolState(FPM\Tester $tester, string $pool, string $expected): void
{
    $response = $tester->request(
        query: 'action=fetch&pool=' . $pool,
        address: '{{ADDR[' . $pool . ']}}',
    );
    $body = trim((string) $response->getBody());
    if ($body !== $expected) {
        throw new RuntimeException(sprintf(
            'Unexpected state for pool %s: expected %s, got %s',
            $pool,
            $expected,
            $body
        ));
    }
}

function seedPool(FPM\Tester $tester, string $pool, string $expected): void
{
    $response = $tester->request(
        query: 'action=seed&pool=' . $pool,
        address: '{{ADDR[' . $pool . ']}}',
    );
    $body = trim((string) $response->getBody());
    if ($body !== $expected) {
        throw new RuntimeException(sprintf(
            'Unexpected seed state for pool %s: expected %s, got %s',
            $pool,
            $expected,
            $body
        ));
    }
}

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.user_cache_shm_size' => '32M',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

seedPool($tester, 'alpha', 'alpha:alpha-value:default');
expectPoolState($tester, 'alpha', 'alpha:alpha-value:default');
expectPoolState($tester, 'beta', 'beta:MISS:default');

seedPool($tester, 'beta', 'beta:beta-value:default');
expectPoolState($tester, 'alpha', 'alpha:alpha-value:default');
expectPoolState($tester, 'beta', 'beta:beta-value:default');

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

echo "Done\n";

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

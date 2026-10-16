--TEST--
FPM: shared graph payloads carry a pin bitmap sized to the pool's pm.max_children
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<'CFG'
[global]
error_log = {{FILE:LOG}}
[small]
listen = {{ADDR[small]}}
pm = static
pm.max_children = 2
catch_workers_output = yes
php_admin_flag[user_cache.enable] = on
php_admin_value[user_cache.shm_size] = 4M
[large]
listen = {{ADDR[large]}}
pm = ondemand
pm.max_children = 200
catch_workers_output = yes
php_admin_flag[user_cache.enable] = on
php_admin_value[user_cache.shm_size] = 4M
CFG;

$code = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('pin-slots');
$value = ['list' => range(1, 16), 'text' => str_repeat('t', 64)];
if (($_GET['action'] ?? '') === 'store') {
    $cache->clear();
    $before = UserCache\Cache::getStatus()->getUsedMemory();
    $cache->store('graph', $value);
    echo UserCache\Cache::getStatus()->getUsedMemory() - $before;
    exit;
}
$fetched = $cache->fetch('graph');
echo json_encode([
    'equal' => $fetched === $value,
    'pinned' => UserCache\Cache::getStatus()->getGraphPinnedReferences() > 0,
]);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: ['user_cache.enable' => '1']);
$tester->expectLogStartNotices();

try {
    $grown = [];
    foreach (['small', 'large'] as $pool) {
        $grown[$pool] = (int) (string) $tester->request('action=store', address: '{{ADDR[' . $pool . ']}}')->getBody();
        echo $pool, ': ', (string) $tester->request(address: '{{ADDR[' . $pool . ']}}')->getBody(), "\n";
    }
    echo 'payload bytes saved by 64 pin slots instead of 256: ', $grown['large'] - $grown['small'], "\n";
} finally {
    $tester->terminate();
    $tester->expectLogTerminatingNotices();
    $tester->close();

    /* Release builds do not collect cycles at shutdown. */
    unset($tester);
    gc_collect_cycles();
}

?>
--EXPECT--
small: {"equal":true,"pinned":true}
large: {"equal":true,"pinned":true}
payload bytes saved by 64 pin slots instead of 256: 24
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

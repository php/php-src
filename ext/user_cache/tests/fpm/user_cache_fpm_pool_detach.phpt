--TEST--
FPM: a worker maps only its own pool's partition segment
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
if (!is_readable('/proc/self/maps')) {
    die('skip requires /proc/self/maps');
}
?>
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
php_admin_value[user_cache.shm_size] = 24M
[beta]
listen = {{ADDR[beta]}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
php_admin_value[user_cache.shm_size] = 20M
EOT;

$code = <<<'PHP'
<?php
$pool = $_GET['pool'];
$cache = UserCache\Cache::getPool('default');
$cache->store('probe', $pool);
$own = UserCache\Cache::getStatus()->getSharedMemorySize();
$sizes = [24 * 1024 * 1024 => 0, 20 * 1024 * 1024 => 0];
foreach (file('/proc/self/maps') as $line) {
    if (!preg_match('~^([0-9a-f]+)-([0-9a-f]+) (\S+) ~', $line, $m) || $m[3][3] !== 's') {
        continue;
    }
    $len = hexdec($m[2]) - hexdec($m[1]);
    if (isset($sizes[$len])) {
        $sizes[$len]++;
    }
}
printf("%s own=%dM alpha-sized=%d beta-sized=%d value=%s\n", $pool, $own >> 20, $sizes[24 * 1024 * 1024], $sizes[20 * 1024 * 1024], $cache->fetch('probe'));
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

foreach (['alpha', 'beta'] as $pool) {
    $response = $tester->request(query: 'pool=' . $pool, address: '{{ADDR[' . $pool . ']}}');
    echo trim((string) $response->getBody()), "\n";
}

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

/* Release builds do not collect cycles at shutdown. */
unset($tester);
gc_collect_cycles();

echo "Done\n";

?>
--EXPECT--
alpha own=24M alpha-sized=1 beta-sized=0 value=alpha
beta own=20M alpha-sized=0 beta-sized=1 value=beta
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

--TEST--
FPM: opcache_reset and opcache_user_cache_reset clear OPcache User Cache partitions
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
$key = 'fpm_reset_shared_key';

if ($action === 'seed') {
    $cache->store($key, $pool . '-value');
} elseif ($action === 'reset') {
    var_dump(opcache_reset());
} elseif ($action === 'user-reset') {
    var_dump(opcache_user_cache_reset());
}

printf("%s:%s\n", $pool, $cache->fetch($key, 'MISS'));
PHP;

function requestBody(FPM\Tester $tester, string $pool, string $action): string
{
    return trim((string) $tester->request(
        query: 'action=' . $action . '&pool=' . $pool,
        address: '{{ADDR[' . $pool . ']}}',
    )->getBody());
}

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.user_cache_shm_size' => '32M',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

var_dump(requestBody($tester, 'alpha', 'seed'));
var_dump(requestBody($tester, 'beta', 'seed'));
var_dump(requestBody($tester, 'alpha', 'fetch'));
var_dump(requestBody($tester, 'beta', 'fetch'));
var_dump(requestBody($tester, 'alpha', 'reset'));
var_dump(requestBody($tester, 'alpha', 'fetch'));
var_dump(requestBody($tester, 'beta', 'fetch'));
var_dump(requestBody($tester, 'alpha', 'seed'));
var_dump(requestBody($tester, 'beta', 'seed'));
var_dump(requestBody($tester, 'alpha', 'fetch'));
var_dump(requestBody($tester, 'beta', 'fetch'));
var_dump(requestBody($tester, 'beta', 'user-reset'));
var_dump(requestBody($tester, 'alpha', 'fetch'));
var_dump(requestBody($tester, 'beta', 'fetch'));

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

?>
--EXPECT--
string(17) "alpha:alpha-value"
string(15) "beta:beta-value"
string(17) "alpha:alpha-value"
string(15) "beta:beta-value"
string(21) "bool(true)
alpha:MISS"
string(10) "alpha:MISS"
string(9) "beta:MISS"
string(17) "alpha:alpha-value"
string(15) "beta:beta-value"
string(17) "alpha:alpha-value"
string(15) "beta:beta-value"
string(20) "bool(true)
beta:MISS"
string(10) "alpha:MISS"
string(9) "beta:MISS"
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

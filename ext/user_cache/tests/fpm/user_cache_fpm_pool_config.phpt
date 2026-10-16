--TEST--
FPM: UserCache storage uses each pool's startup settings without changing master defaults
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<'CFG'
[global]
error_log = {{FILE:LOG}}
[alpha]
listen = {{ADDR[alpha]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
php_flag[user_cache.enable] = on
php_value[user_cache.shm_size] = 3M
php_admin_value[user_cache.shm_size] = 1M
php_admin_value[user_cache.entries_hint] = 127
php_admin_value[user_cache.preferred_memory_model] = posix
[beta]
listen = {{ADDR[beta]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
php_admin_flag[user_cache.enable] = on
php_admin_value[user_cache.shm_size] = 2M
php_admin_value[user_cache.entries_hint] = 255
[disabled]
listen = {{ADDR[disabled]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
php_admin_flag[user_cache.enable] = off
php_admin_value[user_cache.shm_size] = 3M
[inherited]
listen = {{ADDR[inherited]}}
pm = static
pm.max_children = 1
catch_workers_output = yes
CFG;

$code = <<<'PHP'
<?php
$status = UserCache\Cache::getStatus();
ob_start();
phpinfo(INFO_MODULES);
preg_match('/Active memory model\s*(?:<\/td><td class="v">|=>)\s*([a-z0-9-]+)/', ob_get_clean(), $model);
echo json_encode([
    'pool' => fpm_get_status()['pool'] ?? null,
    'pid' => getmypid(),
    'availability' => $status->getAvailability()->name,
    'configured' => $status->getConfiguredMemory(),
    'allocated' => $status->getSharedMemorySize(),
    'capacity' => $status->getEntryCapacity(),
    'preferred' => ini_get_all('user_cache')['user_cache.preferred_memory_model']['local_value'],
    'model' => $model[1] ?? null,
]);
PHP;

foreach ([1, 0] as $masterEnabled) {
    $tester = new FPM\Tester($cfg, $code);
    $tester->start(iniEntries: [
        'user_cache.enable' => (string) $masterEnabled,
        'user_cache.shm_size' => '8M',
        'user_cache.entries_hint' => '0',
    ]);
    $tester->expectLogStartNotices();

    try {
        $states = [];
        foreach (['alpha' => 1, 'beta' => 2, 'disabled' => 3, 'inherited' => 8] as $pool => $size) {
            $response = $tester->request(address: '{{ADDR[' . $pool . ']}}');
            $state = json_decode((string) $response->getBody(), true, flags: JSON_THROW_ON_ERROR);
            $enabled = $pool !== 'disabled' && ($pool !== 'inherited' || $masterEnabled);
            $bytes = $size * 1024 * 1024;
            if ($state['pool'] !== $pool ||
                $state['configured'] !== $bytes ||
                $state['allocated'] !== ($enabled ? $bytes : 0) ||
                $state['availability'] !== ($enabled ? 'Available' : 'DisabledByIni') ||
                $state['preferred'] !== ($pool === 'alpha' ? 'posix' : '') ||
                $state['model'] !== ($enabled ? ($pool === 'alpha' ? 'posix' : 'mmap') : 'none')) {
                throw new RuntimeException($pool . ': ' . json_encode($state));
            }
            if (!$enabled && $state['capacity'] !== 0) {
                throw new RuntimeException($pool . ' allocated a disabled cache');
            }
            $states[$pool] = $state;
        }
        if ($states['alpha']['capacity'] !== 173 || $states['beta']['capacity'] !== 347) {
            throw new RuntimeException('Pool entry hints were not applied');
        }
        if ($masterEnabled && $states['inherited']['capacity'] <= $states['beta']['capacity']) {
            throw new RuntimeException('A pool entry hint changed the master default');
        }
        echo 'master enable=', $masterEnabled, ': pools configured correctly', "\n";
    } finally {
        $tester->terminate();
        $tester->expectLogTerminatingNotices();
        $tester->close();

        /* Release builds do not collect cycles at shutdown. */
        unset($tester);
        gc_collect_cycles();
    }
}

function user_cache_fpm_clamping_master(string $cfg, string $code, array $pools, array $iniEntries): string
{
    $tester = new FPM\Tester($cfg, $code);
    $tester->start(iniEntries: $iniEntries + [
        'user_cache.enable' => '1',
        'user_cache.shm_size' => '8M',
        'user_cache.entries_hint' => '99999999',
        'log_errors' => '1',
        'display_errors' => '0',
    ]);
    $tester->expectLogStartNotices();

    try {
        foreach ($pools as $pool => $capacity) {
            $response = $tester->request(address: '{{ADDR[' . $pool . ']}}');
            $state = json_decode((string) $response->getBody(), true, flags: JSON_THROW_ON_ERROR);
            if ($state['availability'] !== 'Available' || ($capacity !== null && $state['capacity'] !== $capacity)) {
                throw new RuntimeException($pool . ': ' . json_encode($state));
            }
        }
    } finally {
        $tester->terminate();
        $tester->expectLogTerminatingNotices();
        $tester->close();
    }

    ob_start();
    $tester->printLogs();
    $log = ob_get_clean();

    unset($tester);
    gc_collect_cycles();

    return $log;
}

$overrideCfg = <<<'CFG'
[global]
error_log = {{FILE:LOG}}
[alpha]
listen = {{ADDR[alpha]}}
pm = static
pm.max_children = 1
php_admin_value[user_cache.entries_hint] = 127
[beta]
listen = {{ADDR[beta]}}
pm = static
pm.max_children = 1
php_admin_value[user_cache.entries_hint] = 255
CFG;

$masterClamp = 'user_cache.entries_hint is limited to 16777213; clamping';
$capacityClamp = 'user_cache.entries_hint (16777213) exceeds what user_cache.shm_size can index';

echo "Restoring a clamped master entries_hint after the pool overrides:\n";
$fpmLog = user_cache_fpm_clamping_master($overrideCfg, $code, ['alpha' => 173, 'beta' => 347], []);
echo 'FPM log clamp warnings: ', substr_count($fpmLog, $masterClamp), "\n";

$inheritCfg = <<<'CFG'
[global]
error_log = {{FILE:LOG}}
[alpha]
listen = {{ADDR[alpha]}}
pm = static
pm.max_children = 1
php_admin_value[user_cache.entries_hint] = 127
[inherited]
listen = {{ADDR[inherited]}}
pm = static
pm.max_children = 1
CFG;

$phpLog = sys_get_temp_dir() . '/php-user-cache-fpm-pool-config-' . getmypid() . '.log';
@unlink($phpLog);

echo "Master-side warnings with error_log set:\n";
try {
    $fpmLog = user_cache_fpm_clamping_master($inheritCfg, $code, ['alpha' => 173, 'inherited' => null], ['error_log' => $phpLog]);
    $errorLog = (string) @file_get_contents($phpLog);
    echo 'FPM log capacity clamp warnings: ', substr_count($fpmLog, $capacityClamp), "\n";
    echo 'error_log clamp warnings: ', substr_count($errorLog, $masterClamp), ', capacity clamp warnings: ', substr_count($errorLog, $capacityClamp), "\n";
} finally {
    @unlink($phpLog);
}

?>
--EXPECT--
master enable=1: pools configured correctly
master enable=0: pools configured correctly
Restoring a clamped master entries_hint after the pool overrides:
FPM log clamp warnings: 1
Master-side warnings with error_log set:
FPM log capacity clamp warnings: 1
error_log clamp warnings: 1, capacity clamp warnings: 0
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

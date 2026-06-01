--TEST--
FPM: OPcache Static Cache is separated between pools
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

#[OPcache\PinnedStatic]
class FpmPoolSeparatePinnedStatic
{
    public static int $value = 0;
}

class FpmPoolSeparatePinnedProperty
{
    #[OPcache\PinnedStatic]
    public static int $value = 0;
}

class FpmPoolSeparatePinnedMethod
{
    #[OPcache\PinnedStatic]
    public static function value(?int $value = null): int
    {
        static $stored = 0;

        if ($value !== null) {
            $stored = $value;
        }

        return $stored;
    }
}

$action = $_GET['action'] ?? 'fetch';
$pool = $_GET['pool'] ?? 'unknown';
$volatileKey = 'fpm_pool_separate_shared_key';
$pinnedKey = 'fpm_pool_separate_pinned_key';

if ($action === 'seed') {
    $base = $pool === 'alpha' ? 100 : 200;

    OPcache\volatile_store($volatileKey, $pool . '-volatile');
    OPcache\pinned_store($pinnedKey, $pool . '-pinned');
    FpmPoolSeparatePinnedStatic::$value = $base + 1;
    FpmPoolSeparatePinnedProperty::$value = $base + 2;
    FpmPoolSeparatePinnedMethod::value($base + 3);
}

printf(
    "%s:%s:%s:%d:%d:%d\n",
    $pool,
    OPcache\volatile_fetch($volatileKey, 'MISS'),
    OPcache\pinned_fetch($pinnedKey, 'MISS'),
    FpmPoolSeparatePinnedStatic::$value,
    FpmPoolSeparatePinnedProperty::$value,
    FpmPoolSeparatePinnedMethod::value()
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
    'opcache.static_cache.volatile_size_mb' => '32',
    'opcache.static_cache.pinned_size_mb' => '32',
    'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

seedPool($tester, 'alpha', 'alpha:alpha-volatile:alpha-pinned:101:102:103');
expectPoolState($tester, 'alpha', 'alpha:alpha-volatile:alpha-pinned:101:102:103');
expectPoolState($tester, 'beta', 'beta:MISS:MISS:0:0:0');

seedPool($tester, 'beta', 'beta:beta-volatile:beta-pinned:201:202:203');
expectPoolState($tester, 'alpha', 'alpha:alpha-volatile:alpha-pinned:101:102:103');
expectPoolState($tester, 'beta', 'beta:beta-volatile:beta-pinned:201:202:203');

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

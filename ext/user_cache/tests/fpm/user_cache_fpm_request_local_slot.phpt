--TEST--
FPM: UserCache\Cache request-local slots amortize __unserialize per request only
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
class FpmSlotMagicValue
{
    public static int $unserializeCalls = 0;

    public function __construct(public int $value = 0)
    {
    }

    public function __serialize(): array
    {
        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        $this->value = $data['value'];
    }
}

$cache = UserCache\Cache::getPool('default');
$scenario = $_GET['scenario'] ?? 'seed';
$key = 'fpm_request_local_slot_key';

if ($scenario === 'seed') {
    $cache->clear();
    echo $cache->store($key, new FpmSlotMagicValue(41)) ? 'stored' : 'store-failed';
    return;
}

if ($scenario === 'measure') {
    $last = null;
    for ($i = 0; $i < 5; $i++) {
        $last = $cache->fetch($key);
    }
    echo FpmSlotMagicValue::$unserializeCalls, ':', $last->value;
    return;
}

throw new RuntimeException('unknown scenario ' . $scenario);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'user_cache.shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'scenario=seed')->expectBody('stored');

/* Each fetch must run __unserialize(), including repeated fetches in one request. */
$tester->request(query: 'scenario=measure')->expectBody('5:41');
$tester->request(query: 'scenario=measure')->expectBody('5:41');

$tester->terminate();

$tester->close();

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

--TEST--
UserCache\Cache: per-fetch decoding, remember callbacks, decode failures and release of pinned payloads across FPM requests
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[www]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
class CountingMagicValue
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

final class RememberCarrier
{
    public function __construct(public string $name) {}
}

[$section, $step] = explode('_', $_GET['action'], 2);
$cache = UserCache\Cache::getPool($section);

/* __unserialize() runs on every fetch, with no cross-fetch amortisation; static state resets per request */
if ($section === 'unserialize') {
    $key = 'magic_value';

    if ($step === 'seed') {
        $cache->clear();
        echo $cache->store($key, new CountingMagicValue(41)) ? 'stored' : 'store-failed';
        return;
    }

    $last = null;
    for ($i = 0; $i < 5; $i++) {
        $last = $cache->fetch($key);
    }
    echo CountingMagicValue::$unserializeCalls, ':', $last->value;
    return;
}

/* remember() callbacks capturing request-local state are not retained across requests */
if ($section === 'remember') {
    if ($step === 'seed') {
        $cache->clear();

        $token = 'seed-token';
        $carrier = new RememberCarrier('captured-object');
        $resource = tmpfile();
        fwrite($resource, 'request-local-resource');

        $value = $cache->remember('captured', function () use (&$token, $carrier, $resource) {
            return [
                'token' => $token,
                'carrier' => $carrier->name,
                'position' => ftell($resource),
            ];
        });

        var_dump($value);
        echo "seed\n";
        return;
    }

    if ($step === 'hit') {
        $poison = 'second-request';
        $value = $cache->remember('captured', function () use (&$poison) {
            echo "CALLBACK_RAN:$poison\n";
            throw new RuntimeException('remember callback must not run on cache hit');
        });

        var_dump($value);
        echo "hit\n";
        return;
    }

    $cache->delete('captured');
    $value = $cache->remember('captured', function () {
        return 'fresh-after-delete';
    });

    var_dump($value);
    echo "miss\n";
    return;
}

/* Silently corrupt entries are dropped while autoloader exceptions propagate and keep the entry */
if ($section === 'decode') {
    if ($step === 'seed') {
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

    spl_autoload_register(function (string $class): void {
        if ($class === 'UserCacheAutoloadThrows') {
            throw new Exception('autoload failed');
        }
    });

    /* Missing classes cause silent decode failure and entry removal. */
    var_dump($cache->fetch('single', 'DEFAULT'));
    var_dump($cache->has('single'));

    /* Autoloader exceptions propagate and preserve the entry. */
    try {
        $cache->fetch('autoload-single', 'DEFAULT');
        echo "no exception\n";
    } catch (Exception $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
    var_dump($cache->has('autoload-single'));

    /* Drop the corrupt entry before the autoloader aborts the batch. */
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

/* Payloads deleted while this request still pins them are freed when the request ends */
if ($section === 'orphan') {
    if ($step === 'seed') {
        $cache->clear();
        $cache->store('baseline', 0);
        $cache->store('tick', 0);
        $baseline = UserCache\Cache::getStatus()->getFreeMemory();
        $cache->store('baseline', $baseline);

        $pad = str_repeat('o', 3000);
        $held = [];
        for ($i = 0; $i < 40; $i++) {
            $cache->store("o:$i", ['i' => $i, 'pad' => $pad]);
            $held[] = $cache->fetch("o:$i");
        }
        for ($i = 0; $i < 40; $i++) {
            $cache->delete("o:$i");
        }
        echo 'held:', var_export(UserCache\Cache::getStatus()->getFreeMemory() < $baseline, true);
        echo ' pins:', var_export(UserCache\Cache::getStatus()->getGraphPinnedReferences() > 0, true);
        echo ' intact:', var_export($held[39]['pad'] === $pad, true);
        return;
    }

    if ($step === 'check') {
        $status = UserCache\Cache::getStatus();
        echo 'freed:', var_export($status->getFreeMemory() === $cache->fetch('baseline'), true);
        echo ' pins:', $status->getGraphPinnedReferences();
        return;
    }
}

throw new RuntimeException('unknown action ' . $_GET['action']);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'user_cache.enable' => '1',
    'user_cache.shm_size' => '32M',
]);
$tester->expectLogStartNotices();

/* __unserialize() runs on every fetch, with no cross-fetch amortisation; static state resets per request */
$tester->request(query: 'action=unserialize_seed')->expectBody('stored');
$tester->request(query: 'action=unserialize_measure')->expectBody('5:41');
$tester->request(query: 'action=unserialize_measure')->expectBody('5:41');

/* remember() callbacks capturing request-local state are not retained across requests */
$expectedValue =
    "array(3) {\n" .
    "  [\"token\"]=>\n" .
    "  string(10) \"seed-token\"\n" .
    "  [\"carrier\"]=>\n" .
    "  string(15) \"captured-object\"\n" .
    "  [\"position\"]=>\n" .
    "  int(22)\n" .
    "}";

$tester->request(query: 'action=remember_seed')->expectBody(
    $expectedValue . "\n" .
    "seed"
);

$tester->request(query: 'action=remember_hit')->expectBody(
    $expectedValue . "\n" .
    "hit"
);

$tester->request(query: 'action=remember_miss')->expectBody(
    "string(18) \"fresh-after-delete\"\n" .
    "miss"
);

/* Silently corrupt entries are dropped while autoloader exceptions propagate and keep the entry */
$tester->request(query: 'action=decode_seed')->expectBody(
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)\n" .
    "bool(true)"
);

$tester->request(query: 'action=decode_fetch')->expectBody(
    "string(7) \"DEFAULT\"\n" .
    "bool(false)\n" .
    "caught: autoload failed\n" .
    "bool(true)\n" .
    "caught: autoload failed\n" .
    "bool(false)\n" .
    "bool(true)"
);

/* Payloads deleted while this request still pins them are freed when the request ends */
$tester->request(query: 'action=orphan_seed')->expectBody('held:true pins:true intact:true');
$tester->request(query: 'action=orphan_check')->expectBody('freed:true pins:0');

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

/* Release builds do not collect cycles at shutdown. */
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

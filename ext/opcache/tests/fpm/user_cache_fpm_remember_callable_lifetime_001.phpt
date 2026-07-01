--TEST--
FPM: OPcache User Cache remember does not retain callables across requests
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
[opcache]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
final class RememberCarrier
{
    public function __construct(public string $name) {}
}

$cache = new Opcache\UserCache('remember-callable-lifetime');
$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
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

if ($action === 'hit') {
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
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.user_cache_shm_size' => '32M',
]);
$tester->expectLogStartNotices();

$expectedValue =
    "array(3) {\n" .
    "  [\"token\"]=>\n" .
    "  string(10) \"seed-token\"\n" .
    "  [\"carrier\"]=>\n" .
    "  string(15) \"captured-object\"\n" .
    "  [\"position\"]=>\n" .
    "  int(22)\n" .
    "}";

$tester->request(query: 'action=seed')->expectBody(
    $expectedValue . "\n" .
    "seed"
);

$tester->request(query: 'action=hit')->expectBody(
    $expectedValue . "\n" .
    "hit"
);

$tester->request(query: 'action=miss')->expectBody(
    "string(18) \"fresh-after-delete\"\n" .
    "miss"
);

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

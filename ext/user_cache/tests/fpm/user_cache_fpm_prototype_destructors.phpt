--TEST--
UserCache\Cache: prototype eligibility uses the current class destructor after an FPM request changes its definition
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
if ($_GET['action'] === 'seed') {
    class ChangingDestructorValue
    {
        public int $value = 41;
    }
} else {
    class ChangingDestructorValue
    {
        public static int $destructors = 0;
        public int $value = 41;
        public function __destruct()
        {
            self::$destructors++;
        }
    }
}

$cache = UserCache\Cache::getPool('changing-destructor');
if ($_GET['action'] === 'seed') {
    $cache->clear();
    var_dump($cache->store('direct', new ChangingDestructorValue));
    var_dump($cache->store('hidden', new ArrayObject([new ChangingDestructorValue])));
    return;
}

foreach (['direct', 'hidden'] as $key) {
    $valid = true;
    for ($i = 0; $i < 2; $i++) {
        $fetched = $cache->fetch($key);
        $valid = ($key === 'direct' ? $fetched->value : $fetched[0]->value) === 41 && $valid;
        unset($fetched);
    }
    echo $key, ':', ChangingDestructorValue::$destructors, ':', $valid ? 'valid' : 'invalid', "\n";
    $cache->delete($key);
    echo 'after delete:', ChangingDestructorValue::$destructors, "\n";
}
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: ['user_cache.enable' => '1', 'user_cache.shm_size' => '16M']);
$tester->expectLogStartNotices();
for ($i = 0; $i < 2; $i++) {
    $tester->request(query: 'action=seed')->expectBody("bool(true)\nbool(true)");
    $tester->request(query: 'action=fetch')->expectBody(
        "direct:2:valid\nafter delete:2\nhidden:4:valid\nafter delete:4");
}
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

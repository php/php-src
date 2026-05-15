--TEST--
FPM: OPcache PersistentStatic persists across requests
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
#[OPcache\PersistentStatic]
class SharedCounter
{
    public static int $classValue = 0;

    public static function methodValue(): int
    {
        static $value = 0;

        return ++$value;
    }
}

class PropertyCounter
{
    #[OPcache\PersistentStatic]
    public static int $value = 0;
}

class NormalCounter
{
    public static int $classValue = 0;

    public static function methodValue(): int
    {
        static $value = 0;

        return ++$value;
    }
}

echo ++SharedCounter::$classValue, ',', SharedCounter::methodValue(), ',', ++PropertyCounter::$value, ',', ++NormalCounter::$classValue, ',', NormalCounter::methodValue(), "\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
    'opcache.enable' => '1',
    'opcache.static_cache.persistent_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request()->expectBody('1,1,1,1,1');
$tester->request()->expectBody('2,2,2,1,1');

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

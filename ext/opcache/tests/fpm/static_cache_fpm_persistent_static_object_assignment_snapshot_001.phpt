--TEST--
FPM: OPcache PinnedStatic snapshots object assignments without following object property writes
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
pm.max_requests = 1
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
class NestedObjectPropertyState
{
	#[OPcache\PinnedStatic]
	public static ?stdClass $propertyState = null;
}

NestedObjectPropertyState::$propertyState ??= (object) ['count' => 1];
echo NestedObjectPropertyState::$propertyState->count, "\n";
echo OPcache\PinnedCache::info()->entry_count, "\n";

NestedObjectPropertyState::$propertyState->count++;
echo NestedObjectPropertyState::$propertyState->count, "\n";
echo OPcache\PinnedCache::info()->entry_count, "\n";

NestedObjectPropertyState::$propertyState->count = 10;
echo NestedObjectPropertyState::$propertyState->count, "\n";
echo OPcache\PinnedCache::info()->entry_count;
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.pinned_size_mb' => '32',
	'opcache.optimization_level' => '0',
	'opcache.file_update_protection' => '0',
	'opcache.jit' => '0',
]);
$tester->expectLogStartNotices();

$tester->request()->expectBody("1\n1\n2\n1\n10\n1");

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

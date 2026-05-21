--TEST--
FPM: OPcache PinnedStatic publishes nested array writes for property-scoped state
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
class NestedArrayPropertyState
{
	#[OPcache\PinnedStatic]
	public static array $propertyState = [];
}

NestedArrayPropertyState::$propertyState = ['numbers' => [1]];
echo json_encode(NestedArrayPropertyState::$propertyState['numbers']), "\n";
echo OPcache\pinned_cache_info()->entry_count, "\n";

NestedArrayPropertyState::$propertyState['numbers'][] = 2;
echo json_encode(NestedArrayPropertyState::$propertyState['numbers']), "\n";
echo OPcache\pinned_cache_info()->entry_count;
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

$tester->request()->expectBody("[1]\n1\n[1,2]\n1");

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

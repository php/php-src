--TEST--
FPM: OPcache PinnedStatic class blob persists across requests
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
#[OPcache\PinnedStatic]
class CombinedBlobState
{
	public static int $propertyCount = 0;
	public static array $propertyBag = [];

	public static function nextMethod(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$request = (int) ($_GET['request'] ?? 1);
if ($request === 1) {
	CombinedBlobState::$propertyCount++;
	CombinedBlobState::$propertyBag['numbers'][] = 10;
	echo CombinedBlobState::$propertyCount, ',', CombinedBlobState::nextMethod();
	return;
}

echo CombinedBlobState::$propertyCount, ',', CombinedBlobState::$propertyBag['numbers'][0], ',', CombinedBlobState::nextMethod(), ',', OPcache\PinnedCache::info()->entry_count, "\n";

CombinedBlobState::$propertyCount++;
CombinedBlobState::$propertyBag['numbers'][] = 11;
echo CombinedBlobState::$propertyCount, ',', array_sum(CombinedBlobState::$propertyBag['numbers']), ',', CombinedBlobState::nextMethod();
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.pinned_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'request=1')->expectBody('1,1');
$tester->request(query: 'request=2')->expectBody("1,10,2,1\n2,21,3");

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

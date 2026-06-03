--TEST--
FPM: OPcache volatile cache request-local lookup cache sees same-request updates
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
function fetch_or_miss(string $key): string
{
	return OPcache\VolatileCache::getInstance('default')->fetch($key, 'MISS');
}

$scenario = $_GET['scenario'] ?? 'hit_store';
$key = 'fpm_lookup_cache_' . $scenario . '_key';
opcache_static_cache_volatile_reset();

if ($scenario === 'hit_store') {
	OPcache\VolatileCache::getInstance('default')->store($key, 'old');
	$first = fetch_or_miss($key);
	OPcache\VolatileCache::getInstance('default')->store($key, 'new');
	echo $first, "\n", fetch_or_miss($key);
	return;
}

if ($scenario === 'miss_store') {
	$first = fetch_or_miss($key);
	OPcache\VolatileCache::getInstance('default')->store($key, 'created');
	echo $first, "\n", fetch_or_miss($key);
	return;
}

if ($scenario === 'hit_delete') {
	OPcache\VolatileCache::getInstance('default')->store($key, 'old');
	$first = fetch_or_miss($key);
	OPcache\VolatileCache::getInstance('default')->delete($key);
	echo $first, "\n", fetch_or_miss($key);
	return;
}

if ($scenario === 'hit_clear') {
	OPcache\VolatileCache::getInstance('default')->store($key, 'old');
	$first = fetch_or_miss($key);
	opcache_static_cache_volatile_reset();
	echo $first, "\n", fetch_or_miss($key);
	return;
}

throw new RuntimeException('unknown scenario ' . $scenario);
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.volatile_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'scenario=hit_store')->expectBody("old\nnew");
$tester->request(query: 'scenario=miss_store')->expectBody("MISS\ncreated");
$tester->request(query: 'scenario=hit_delete')->expectBody("old\nMISS");
$tester->request(query: 'scenario=hit_clear')->expectBody("old\nMISS");

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

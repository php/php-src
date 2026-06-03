--TEST--
FPM: OPcache volatile cache keeps plain userland objects shared until mutation across requests
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
clear_env = no
EOT;

$code = <<<'PHP'
<?php
class LargePayload
{
	public function __construct(
		public array $rows,
		public string $label,
	) {}
}

function build_rows(): array
{
	$rows = [];

	for ($i = 0; $i < 12000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat(chr(65 + ($i % 26)), 96),
			'flags' => [$i, $i + 1, $i + 2, $i + 3],
		];
	}

	return $rows;
}

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	opcache_static_cache_volatile_reset();
	var_dump(OPcache\VolatileCache::getInstance('default')->store('materialization_plain_payload', new LargePayload(build_rows(), 'plain')));
	echo "seed\n";
	return;
}

$before = memory_get_usage();
$fetched = OPcache\VolatileCache::getInstance('default')->fetch('materialization_plain_payload');
$readOnly = $fetched->rows[100]['text'];
$afterFetch = memory_get_usage();
$fetched->rows[100]['text'] = 'changed';
$afterMutate = memory_get_usage();
$usesZendAlloc = getenv('USE_ZEND_ALLOC') !== '0';

var_dump(!$usesZendAlloc || ($afterFetch - $before) < 262144);
var_dump($fetched->rows[100]['text'] === 'changed' && (!$usesZendAlloc || ($afterMutate - $afterFetch) > 131072));
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.volatile_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
	"bool(true)\n" .
	"seed"
);

$tester->request(query: 'action=fetch')->expectBody(
	"bool(true)\n" .
	"bool(true)"
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

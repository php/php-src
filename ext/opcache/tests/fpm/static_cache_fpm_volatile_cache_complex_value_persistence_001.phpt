--TEST--
FPM: OPcache volatile cache persists complex values across requests
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
class SimpleUser
{
	public function __construct(
		public string $name,
		public int $age,
	) {}
}

class SerUser
{
	private int $id;
	private string $name;

	public function __construct(int $id, string $name)
	{
		$this->id = $id;
		$this->name = $name;
	}

	public function __serialize(): array
	{
		return ['id' => $this->id, 'name' => $this->name];
	}

	public function __unserialize(array $data): void
	{
		$this->id = $data['id'];
		$this->name = $data['name'];
	}

	public function info(): string
	{
		return $this->id . ':' . $this->name;
	}
}

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	$gap = [];
	$gap[4] = 'seed';
	unset($gap[4]);

	$payload = [
		'props' => new SimpleUser('Alice', 30),
		'serialize' => new SerUser(7, 'Bob'),
		'internal' => new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
		'gap' => $gap,
	];

	$shared = new stdClass();
	$shared->value = 42;

	$refs = ['value' => 1];
	$refs['alias'] =& $refs['value'];

	opcache_static_cache_volatile_reset();
	var_dump(OPcache\VolatileCache::getInstance('default')->store('complex', $payload));
	var_dump(OPcache\VolatileCache::getInstance('default')->store('shared_pair', [$shared, $shared]));
	var_dump(OPcache\VolatileCache::getInstance('default')->store('refs', $refs));
	echo "seed\n";
	return;
}

$complex = OPcache\VolatileCache::getInstance('default')->fetch('complex');
$complex['gap'][] = 'tail';
echo $complex['props']->name, ',', $complex['props']->age, ',', $complex['serialize']->info(), ',', $complex['internal']->format('Y-m-d H:i:s'), ',', array_key_last($complex['gap']), "\n";

$pair = OPcache\VolatileCache::getInstance('default')->fetch('shared_pair');
var_dump(spl_object_id($pair[0]) === spl_object_id($pair[1]));

$refs = OPcache\VolatileCache::getInstance('default')->fetch('refs');
$refs['alias'] = 7;
var_dump($refs['value']);

echo "fetch\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.volatile_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=seed')->expectBody(
	"bool(true)\n" .
	"bool(true)\n" .
	"bool(true)\n" .
	"seed"
);

$tester->request(query: 'action=fetch')->expectBody(
	"Alice,30,7:Bob,2026-06-15 09:30:00,5\n" .
	"bool(true)\n" .
	"int(7)\n" .
	"fetch"
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

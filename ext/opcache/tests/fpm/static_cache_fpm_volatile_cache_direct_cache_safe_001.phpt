--TEST--
FPM: OPcache __DirectCacheSafe subclasses survive requests, safe serializer overrides stay direct, and wakeup hooks fallback
--EXTENSIONS--
opcache
spl
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
class EventDateTime extends DateTime
{
	private string $label;
	protected int $revision;

	public function __construct(string $time, DateTimeZone $timezone, string $label, int $revision)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
		$this->revision = $revision;
	}

	public function describe(): string
	{
		return $this->label . ':' . $this->revision;
	}
}

class LabelIterator extends ArrayIterator
{
}

class TaggedCollection extends ArrayObject
{
	private string $type;

	public function __construct(array $data, string $type, string $iteratorClass)
	{
		parent::__construct($data, 0, $iteratorClass);
		$this->type = $type;
	}

	public function type(): string
	{
		return $this->type;
	}
}

class CustomSerializedDateTime extends DateTime
{
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		return parent::__serialize() + ['label' => $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->label = $data['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

class WakefulSerializedDateTime extends DateTime
{
	public static int $unserializeCount = 0;

	private string $label;

	public function __construct(string $time, DateTimeZone $timezone, string $label)
	{
		parent::__construct($time, $timezone);
		$this->label = $label;
	}

	public function __serialize(): array
	{
		return parent::__serialize() + ['label' => $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		parent::__unserialize($data);
		$this->label = $data['label'];
	}

	public function __sleep(): array
	{
		return ['label'];
	}

	public function label(): string
	{
		return $this->label;
	}
}

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	OPcache\volatile_clear();

	$event = new EventDateTime('2026-06-15 09:30:00.123456', new DateTimeZone('Europe/Paris'), 'launch', 7);
	$collection = new TaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', LabelIterator::class);
	$fallback = new CustomSerializedDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'fallback');
	$wakeful = new WakefulSerializedDateTime('2026-06-15 12:15:00.987654', new DateTimeZone('UTC'), 'fallback');

	var_dump(OPcache\volatile_store('safe_direct_event_datetime', $event));
	var_dump(OPcache\volatile_store('safe_direct_tagged_collection', $collection));
	var_dump(OPcache\volatile_store('safe_direct_custom_datetime', $fallback));
	var_dump(OPcache\volatile_store('safe_direct_wakeful_datetime', $wakeful));
	echo "seed\n";
	return;
}

$event = OPcache\volatile_fetch('safe_direct_event_datetime');
$collection = OPcache\volatile_fetch('safe_direct_tagged_collection');
$fallback = OPcache\volatile_fetch('safe_direct_custom_datetime');
$wakeful = OPcache\volatile_fetch('safe_direct_wakeful_datetime');
$iterator = $collection->getIterator();

echo $event->format('Y-m-d H:i:s.u e'), ',', $event->describe(), "\n";
echo $collection->type(), ',', ($iterator instanceof LabelIterator ? 'LabelIterator' : get_debug_type($iterator)), ',', $collection['alpha'], ',', $collection['beta'], "\n";
echo $fallback->format('Y-m-d H:i:s.u e'), ',', $fallback->label(), ',', CustomSerializedDateTime::$unserializeCount, "\n";
echo $wakeful->format('Y-m-d H:i:s.u e'), ',', $wakeful->label(), ',', WakefulSerializedDateTime::$unserializeCount, "\n";
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
	"bool(true)\n" .
	"seed"
);

$tester->request(query: 'action=fetch')->expectBody(
	"2026-06-15 09:30:00.123456 Europe/Paris,launch:7\n" .
	"metric,LabelIterator,10,20\n" .
	"2026-06-15 10:45:00.654321 UTC,fallback,0\n" .
	"2026-06-15 12:15:00.987654 UTC,fallback,1"
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

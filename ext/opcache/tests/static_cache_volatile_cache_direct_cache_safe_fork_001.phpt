--TEST--
OPcache direct cache subclasses survive forked fetch and safe serializer overrides stay direct
--EXTENSIONS--
opcache
pcntl
spl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
	die('skip pcntl_fork() not available');
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
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

$readyFile = sys_get_temp_dir() . '/opcache_volatile_cache_direct_cache_safe_fork_' . getmypid() . '.ready';
@unlink($readyFile);

$pid = pcntl_fork();
if ($pid === -1) {
	throw new RuntimeException('pcntl_fork() failed');
}

if ($pid === 0) {
	$deadline = microtime(true) + 5.0;

	while (!file_exists($readyFile)) {
		if (microtime(true) >= $deadline) {
			fwrite(STDERR, "parent did not signal readiness\n");
			exit(1);
		}

		usleep(1000);
	}

	$event = OPcache\volatile_fetch('safe_direct_event_datetime');
	$collection = OPcache\volatile_fetch('safe_direct_tagged_collection');
	$fallback = OPcache\volatile_fetch('safe_direct_custom_datetime');
	$iterator = $collection->getIterator();

	echo $event->format('Y-m-d H:i:s.u e'), ',', $event->describe(), "\n";
	echo $collection->type(), ',', ($iterator instanceof LabelIterator ? 'LabelIterator' : get_debug_type($iterator)), ',', $collection['alpha'], ',', $collection['beta'], "\n";
	echo $fallback->format('Y-m-d H:i:s.u e'), ',', $fallback->label(), ',', CustomSerializedDateTime::$unserializeCount, "\n";
	exit(0);
}

$event = new EventDateTime('2026-06-15 09:30:00.123456', new DateTimeZone('Europe/Paris'), 'launch', 7);
$collection = new TaggedCollection(['alpha' => 10, 'beta' => 20], 'metric', LabelIterator::class);
$fallback = new CustomSerializedDateTime('2026-06-15 10:45:00.654321', new DateTimeZone('UTC'), 'fallback');

if (!OPcache\volatile_store('safe_direct_event_datetime', $event)) {
	throw new RuntimeException('Failed to store safe_direct_event_datetime');
}
if (!OPcache\volatile_store('safe_direct_tagged_collection', $collection)) {
	throw new RuntimeException('Failed to store safe_direct_tagged_collection');
}
if (!OPcache\volatile_store('safe_direct_custom_datetime', $fallback)) {
	throw new RuntimeException('Failed to store safe_direct_custom_datetime');
}

file_put_contents($readyFile, 'ready');
pcntl_waitpid($pid, $status);
@unlink($readyFile);

?>
--EXPECT--
2026-06-15 09:30:00.123456 Europe/Paris,launch:7
metric,LabelIterator,10,20
2026-06-15 10:45:00.654321 UTC,fallback,0
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_volatile_cache_direct_cache_safe_fork_[0-9]*.ready') ?: [] as $path) {
	@unlink($path);
}
?>

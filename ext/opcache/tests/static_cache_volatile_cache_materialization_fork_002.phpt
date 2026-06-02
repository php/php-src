--TEST--
OPcache volatile cache should isolate nested post-mutation fetches with direct cache properties
--EXTENSIONS--
opcache
pcntl
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

class LargePayload
{
	public function __construct(
		public array $rows,
		public string $label,
	) {}
}

class WrappedPayload
{
	public function __construct(
		public LargePayload $payload,
		public DateTimeImmutable $timestamp,
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

$readyFile = sys_get_temp_dir() . '/opcache_volatile_cache_materialization_nested_' . getmypid() . '.ready';
@unlink($readyFile);

$pid = pcntl_fork();
if ($pid === -1) {
	throw new RuntimeException('pcntl_fork() failed');
}

if ($pid === 0) {
	$deadline = microtime(true) + 5.0;
	$expectedText = str_repeat(chr(65 + (100 % 26)), 96);
	$expectedPrefix = '2026-06-15T09:30:00+00:00|';
	$canMeasureRequestAllocations = getenv('USE_ZEND_ALLOC') !== '0';

	while (!file_exists($readyFile)) {
		if (microtime(true) >= $deadline) {
			fwrite(STDERR, "parent did not signal readiness\n");
			exit(1);
		}

		usleep(1000);
	}

	$before = memory_get_usage();
	$fetched = OPcache\VolatileCache::get('materialization_nested_payload');
	$readOnly = $fetched->timestamp->format(DateTimeInterface::ATOM) . '|' . $fetched->payload->rows[100]['text'];
	$afterFetch = memory_get_usage();
	$fetched->payload->rows[100]['text'] = 'changed';
	$afterMutate = memory_get_usage();
	$secondFetch = OPcache\VolatileCache::get('materialization_nested_payload');

	var_dump($readOnly === $expectedPrefix . $expectedText
		&& (!$canMeasureRequestAllocations || ($afterFetch - $before) < 262144));
	var_dump($fetched->payload->rows[100]['text'] === 'changed'
		&& $secondFetch->payload->rows[100]['text'] === $expectedText
		&& (!$canMeasureRequestAllocations || ($afterMutate - $afterFetch) > 131072));
	exit(0);
}

$payload = new WrappedPayload(
	new LargePayload(build_rows(), 'nested'),
	new DateTimeImmutable('2026-06-15 09:30:00', new DateTimeZone('UTC')),
);

if (!OPcache\VolatileCache::set('materialization_nested_payload', $payload)) {
	throw new RuntimeException('Failed to store materialization_nested_payload');
}

file_put_contents($readyFile, 'ready');
pcntl_waitpid($pid, $status);
@unlink($readyFile);

$fetched = OPcache\VolatileCache::get('materialization_nested_payload');
var_dump($fetched->payload->rows[100]['text'] === str_repeat(chr(65 + (100 % 26)), 96));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_volatile_cache_materialization_nested_[0-9]*.ready') ?: [] as $path) {
	@unlink($path);
}
?>

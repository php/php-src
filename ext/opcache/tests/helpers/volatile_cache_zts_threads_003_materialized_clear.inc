<?php

class ZtsClearPayload
{
	public function __construct(public array $rows) {}
}

function build_zts_clear_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 8000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($prefix, 64),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
}

function wait_for_file(string $path): void
{
	$deadline = microtime(true) + 5.0;

	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			throw new RuntimeException("timed out waiting for {$path}");
		}

		usleep(1000);
	}
}

function cleanup_files(string ...$paths): void
{
	foreach ($paths as $path) {
		@unlink($path);
	}
}

$key = 'zts_materialized_clear_payload';
$overwriteKey = 'zts_materialized_clear_overwrite';
$readyFile = sys_get_temp_dir() . '/opcache_zts_materialized_clear.ready';
$doneFile = sys_get_temp_dir() . '/opcache_zts_materialized_clear.done';

if ($mode === 'init') {
	cleanup_files($readyFile, $doneFile);
	OPcache\volatile_clear();
	return OPcache\volatile_store($key, new ZtsClearPayload(build_zts_clear_rows('T', 5)));
}

if ($mode === 'writer') {
	wait_for_file($readyFile);
	OPcache\volatile_clear();
	if (!OPcache\volatile_store($overwriteKey, new ZtsClearPayload(build_zts_clear_rows('X', 7)))) {
		return false;
	}
	file_put_contents($doneFile, 'done');
	return true;
}

$fetched = OPcache\volatile_fetch($key);
file_put_contents($readyFile, 'ready');
wait_for_file($doneFile);
$before = $fetched->rows[123]['text'];
$fetched->rows[123]['text'] = 'changed after clear';
$after = $fetched->rows[123]['text'];
$nested = $fetched->rows[123]['nested']['value'];

$refetch = OPcache\volatile_fetch($key, 'MISS') === 'MISS' ? 'MISS' : 'HIT';

cleanup_files($readyFile, $doneFile);

return $before === str_repeat('T', 64)
	&& $after === 'changed after clear'
	&& $nested === 615
	&& $refetch === 'MISS';

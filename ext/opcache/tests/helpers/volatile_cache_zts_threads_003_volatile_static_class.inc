<?php

#[OPcache\VolatileStatic]
class ZtsVolatileStaticClassPayload
{
	public static mixed $payload = null;
	public static ?string $kind = null;

	public static function resolve(string $kind): bool
	{
		if (self::$kind !== $kind || self::$payload === null) {
			self::$kind = $kind;
			self::$payload = build_zts_volatile_static_payload($kind);
		}

		return self::$payload['rows'][123]['text'] === str_repeat($kind, 64)
			&& self::$payload['rows'][123]['nested']['value'] === 123 * strlen($kind);
	}
}

function build_zts_volatile_static_payload(string $kind): array
{
	$rows = [];

	for ($i = 0; $i < 8000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($kind, 64),
			'nested' => ['value' => $i * strlen($kind)],
		];
	}

	return [
		'kind' => $kind,
		'rows' => $rows,
	];
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

$readyFile = sys_get_temp_dir() . '/opcache_zts_volatile_static_class.ready';
$doneFile = sys_get_temp_dir() . '/opcache_zts_volatile_static_class.done';

if ($mode === 'init') {
	cleanup_files($readyFile, $doneFile);
	OPcache\volatile_clear();
	return true;
}

if ($mode === 'writer') {
	wait_for_file($readyFile);
	$result = ZtsVolatileStaticClassPayload::resolve('writer');
	file_put_contents($doneFile, 'done');

	return $result;
}

$result = ZtsVolatileStaticClassPayload::resolve('reader');
file_put_contents($readyFile, 'ready');
wait_for_file($doneFile);
usleep(50000);

for ($i = 0; $i < 100; $i++) {
	$result = $result && ZtsVolatileStaticClassPayload::resolve('reader');
}

cleanup_files($readyFile, $doneFile);

return $result;
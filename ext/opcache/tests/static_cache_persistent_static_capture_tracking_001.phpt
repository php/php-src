--TEST--
OPcache static attribute decoded graph tracking follows publication kind
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_capture_tracking_001.php', <<<'PHP'
<?php

class CaptureTrackingChild
{
	public function __construct(public array $items = [])
	{
	}
}

class CaptureTrackingPayload
{
	public function __construct(
		public string $name,
		public array $rows,
		public CaptureTrackingChild $child,
	) {
	}
}

function capture_tracking_payload(string $name): CaptureTrackingPayload
{
	return new CaptureTrackingPayload(
		$name,
		[
			'events' => [],
			'sparse' => [0 => new CaptureTrackingChild(['seed']), 3 => new CaptureTrackingChild(['gap'])],
		],
		new CaptureTrackingChild(),
	);
}

class CaptureTrackingCachedMethodA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): CaptureTrackingPayload
	{
		static $value = null;

		$value ??= capture_tracking_payload('cached_method_a');
		return $value;
	}
}

class CaptureTrackingCachedMethodB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): CaptureTrackingPayload
	{
		static $value = null;

		$value ??= capture_tracking_payload('cached_method_b');
		return $value;
	}
}

class CaptureTrackingCachedPropertyA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?CaptureTrackingPayload $value = null;

	public static function value(): CaptureTrackingPayload
	{
		self::$value ??= capture_tracking_payload('cached_property_a');
		return self::$value;
	}
}

class CaptureTrackingCachedPropertyB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?CaptureTrackingPayload $value = null;

	public static function value(): CaptureTrackingPayload
	{
		self::$value ??= capture_tracking_payload('cached_property_b');
		return self::$value;
	}
}

class CaptureTrackingPinnedMethodA
{
	#[OPcache\PinnedStatic]
	public static function value(): CaptureTrackingPayload
	{
		static $value = null;

		$value ??= capture_tracking_payload('pinned_method_a');
		return $value;
	}
}

class CaptureTrackingPinnedMethodB
{
	#[OPcache\PinnedStatic]
	public static function value(): CaptureTrackingPayload
	{
		static $value = null;

		$value ??= capture_tracking_payload('pinned_method_b');
		return $value;
	}
}

class CaptureTrackingPinnedPropertyA
{
	#[OPcache\PinnedStatic]
	public static ?CaptureTrackingPayload $value = null;

	public static function value(): CaptureTrackingPayload
	{
		self::$value ??= capture_tracking_payload('pinned_property_a');
		return self::$value;
	}
}

class CaptureTrackingPinnedPropertyB
{
	#[OPcache\PinnedStatic]
	public static ?CaptureTrackingPayload $value = null;

	public static function value(): CaptureTrackingPayload
	{
		self::$value ??= capture_tracking_payload('pinned_property_b');
		return self::$value;
	}
}

function capture_tracking_pair(string $kind): array
{
	return match ($kind) {
		'cached_method' => [CaptureTrackingCachedMethodA::value(), CaptureTrackingCachedMethodB::value()],
		'cached_property' => [CaptureTrackingCachedPropertyA::value(), CaptureTrackingCachedPropertyB::value()],
		'pinned_method' => [CaptureTrackingPinnedMethodA::value(), CaptureTrackingPinnedMethodB::value()],
		'pinned_property' => [CaptureTrackingPinnedPropertyA::value(), CaptureTrackingPinnedPropertyB::value()],
		default => throw new RuntimeException('unknown kind'),
	};
}

function capture_tracking_mutate(CaptureTrackingPayload $payload, string $tag): void
{
	$payload->rows['events'][] = $tag;
	$payload->rows['sparse'][3]->items[] = $tag;
	$payload->child->items[] = $tag;
}

function capture_tracking_dump(string $kind): void
{
	[$first, $second] = capture_tracking_pair($kind);
	echo $kind, ':',
		count($first->rows['events']), ',',
		count($first->rows['sparse'][3]->items), ',',
		count($first->child->items), '|',
		count($second->rows['events']), ',',
		count($second->rows['sparse'][3]->items), ',',
		count($second->child->items), "\n";
}

$action = $_GET['action'] ?? 'read';
$kind = $_GET['kind'] ?? 'cached_method';

if ($action === 'reset') {
	OPcache\VolatileCache::clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	[$first, $second] = capture_tracking_pair($kind);
	capture_tracking_mutate($first, 'seed-a');
	capture_tracking_mutate($second, 'seed-b');
	capture_tracking_dump($kind);
	return;
}

if ($action === 'mutate_after_fetch') {
	[$first, $second] = capture_tracking_pair($kind);
	capture_tracking_mutate($first, 'fetch-a');
	capture_tracking_mutate($second, 'fetch-b');
	capture_tracking_dump($kind);
	return;
}

if ($action === 'read') {
	capture_tracking_dump($kind);
	return;
}

throw new RuntimeException('unknown action');
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_capture_tracking_001.php';
foreach (['cached_method', 'cached_property', 'pinned_method', 'pinned_property'] as $kind) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=mutate_after_fetch&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_capture_tracking_001.php');
?>
--EXPECT--
reset
cached_method:1,2,1|1,2,1
cached_method:2,3,2|2,3,2
cached_method:2,3,2|2,3,2
reset
cached_property:1,2,1|1,2,1
cached_property:2,3,2|2,3,2
cached_property:2,3,2|2,3,2
reset
pinned_method:1,2,1|1,2,1
pinned_method:1,2,1|1,2,1
pinned_method:0,1,0|0,1,0
reset
pinned_property:1,2,1|1,2,1
pinned_property:1,2,1|1,2,1
pinned_property:0,1,0|0,1,0

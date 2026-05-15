--TEST--
OPcache VolatileStatic strategies control publication timing without changing volatile-cache backend
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$defaultAttribute = new OPcache\VolatileStatic();
$trackingAttribute = new OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking);
$ttlAttribute = new OPcache\VolatileStatic(ttl: 1);
echo 'attribute=', $defaultAttribute->ttl, ':', $defaultAttribute->strategy->value, ',', $trackingAttribute->ttl, ':', $trackingAttribute->strategy->value, ',', $trackingAttribute->strategy->name, ',', $ttlAttribute->ttl, ':', $ttlAttribute->strategy->value, "\n";
echo 'cases=', implode(',', array_map(static fn (OPcache\CacheStrategy $case): string => $case->name . ':' . $case->value, OPcache\CacheStrategy::cases())), "\n";

try {
	new OPcache\VolatileStatic(ttl: -1);
} catch (ValueError) {
	echo "ttl-value-error\n";
}

file_put_contents(__DIR__ . '/volatile_static_strategy_001.php', <<<'PHP'
<?php

class CachedStrategyPayload
{
	public array $events = [];

	public function __construct(public string $name)
	{
	}
}

function cached_strategy_payload(string $name): CachedStrategyPayload
{
	return new CachedStrategyPayload($name);
}

function cached_strategy_mutate(CachedStrategyPayload $payload): void
{
	$payload->events[] = 'x';
}

#[OPcache\VolatileStatic]
class CachedStrategyClassDefault
{
	public static ?CachedStrategyPayload $property = null;

	public static function property(): CachedStrategyPayload
	{
		self::$property ??= cached_strategy_payload('class_default_property');
		return self::$property;
	}

	public static function method(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('class_default_method');
		return $value;
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Immediate)]
class CachedStrategyClassImmediate
{
	public static ?CachedStrategyPayload $property = null;

	public static function property(): CachedStrategyPayload
	{
		self::$property ??= cached_strategy_payload('class_immediate_property');
		return self::$property;
	}

	public static function method(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('class_immediate_method');
		return $value;
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class CachedStrategyClassTracking
{
	public static ?CachedStrategyPayload $property = null;

	public static function property(): CachedStrategyPayload
	{
		self::$property ??= cached_strategy_payload('class_tracking_property');
		return self::$property;
	}

	public static function method(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('class_tracking_method');
		return $value;
	}
}

class CachedStrategyPropertyDefault
{
	#[OPcache\VolatileStatic]
	public static ?CachedStrategyPayload $value = null;

	public static function value(): CachedStrategyPayload
	{
		self::$value ??= cached_strategy_payload('property_default');
		return self::$value;
	}
}

class CachedStrategyPropertyImmediate
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Immediate)]
	public static ?CachedStrategyPayload $value = null;

	public static function value(): CachedStrategyPayload
	{
		self::$value ??= cached_strategy_payload('property_immediate');
		return self::$value;
	}
}

class CachedStrategyPropertyTracking
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?CachedStrategyPayload $value = null;

	public static function value(): CachedStrategyPayload
	{
		self::$value ??= cached_strategy_payload('property_tracking');
		return self::$value;
	}
}

class CachedStrategyMethodDefault
{
	#[OPcache\VolatileStatic]
	public static function value(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('method_default');
		return $value;
	}
}

class CachedStrategyMethodImmediate
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Immediate)]
	public static function value(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('method_immediate');
		return $value;
	}
}

class CachedStrategyMethodTracking
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): CachedStrategyPayload
	{
		static $value = null;

		$value ??= cached_strategy_payload('method_tracking');
		return $value;
	}
}

function cached_strategy_values(string $case): array
{
	return match ($case) {
		'class_default' => [CachedStrategyClassDefault::property(), CachedStrategyClassDefault::method()],
		'class_immediate' => [CachedStrategyClassImmediate::property(), CachedStrategyClassImmediate::method()],
		'class_tracking' => [CachedStrategyClassTracking::property(), CachedStrategyClassTracking::method()],
		'property_default' => [CachedStrategyPropertyDefault::value()],
		'property_immediate' => [CachedStrategyPropertyImmediate::value()],
		'property_tracking' => [CachedStrategyPropertyTracking::value()],
		'method_default' => [CachedStrategyMethodDefault::value()],
		'method_immediate' => [CachedStrategyMethodImmediate::value()],
		'method_tracking' => [CachedStrategyMethodTracking::value()],
		default => throw new RuntimeException('unknown case'),
	};
}

function cached_strategy_dump(string $case, array $values): void
{
	echo $case, ':', implode('|', array_map(
		static fn (CachedStrategyPayload $payload): int => count($payload->events),
		$values,
	)), "\n";
}

$action = $_GET['action'] ?? 'read';
$case = $_GET['case'] ?? 'class_default';

if ($action === 'reset') {
	OPcache\volatile_clear();
	OPcache\persistent_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$values = cached_strategy_values($case);

if ($action === 'seed' || $action === 'mutate_after_fetch') {
	foreach ($values as $value) {
		cached_strategy_mutate($value);
	}
}

cached_strategy_dump($case, $values);

if ($action === 'read') {
	echo 'cache=', OPcache\volatile_cache_info()['entry_count'], ',', OPcache\persistent_cache_info()['entry_count'], "\n";
}
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.persistent_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_strategy_001.php';
$cases = [
	'class_default',
	'class_immediate',
	'class_tracking',
	'property_default',
	'property_immediate',
	'property_tracking',
	'method_default',
	'method_immediate',
	'method_tracking',
];

foreach ($cases as $case) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed&case=' . $case);
	echo file_get_contents($base . '?action=read&case=' . $case);
	echo file_get_contents($base . '?action=mutate_after_fetch&case=' . $case);
	echo file_get_contents($base . '?action=read&case=' . $case);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_strategy_001.php');
?>
--EXPECT--
attribute=0:0,0:1,Tracking,1:0
cases=Immediate:0,Tracking:1
ttl-value-error
reset
class_default:1|1
class_default:0|0
cache=1,0
class_default:1|1
class_default:0|0
cache=1,0
reset
class_immediate:1|1
class_immediate:0|0
cache=1,0
class_immediate:1|1
class_immediate:0|0
cache=1,0
reset
class_tracking:1|1
class_tracking:1|1
cache=1,0
class_tracking:2|2
class_tracking:2|2
cache=1,0
reset
property_default:1
property_default:0
cache=1,0
property_default:1
property_default:0
cache=1,0
reset
property_immediate:1
property_immediate:0
cache=1,0
property_immediate:1
property_immediate:0
cache=1,0
reset
property_tracking:1
property_tracking:1
cache=1,0
property_tracking:2
property_tracking:2
cache=1,0
reset
method_default:1
method_default:0
cache=1,0
method_default:1
method_default:0
cache=1,0
reset
method_immediate:1
method_immediate:0
cache=1,0
method_immediate:1
method_immediate:0
cache=1,0
reset
method_tracking:1
method_tracking:1
cache=1,0
method_tracking:2
method_tracking:2
cache=1,0

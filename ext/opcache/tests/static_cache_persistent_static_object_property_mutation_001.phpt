--TEST--
OPcache static attributes handle object property array writes by publication kind
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_object_property_mutation_001.php', <<<'PHP'
<?php
class StaticAttributePlainObject
{
	public array $bag = [];
}

class StaticAttributeCustomImmutableDate extends DateTimeImmutable
{
	public array $bag = [];
}

class VolatileStaticPlainMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): StaticAttributePlainObject
	{
		static $object = null;

		$object ??= new StaticAttributePlainObject();
		return $object;
	}
}

class VolatileStaticSafeDirectMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): StaticAttributeCustomImmutableDate
	{
		static $object = null;

		$object ??= new StaticAttributeCustomImmutableDate('@0');
		return $object;
	}
}

class PinnedStaticPlainMethodState
{
	#[OPcache\PinnedStatic]
	public static function value(): StaticAttributePlainObject
	{
		static $object = null;

		$object ??= new StaticAttributePlainObject();
		return $object;
	}
}

class PinnedStaticSafeDirectMethodState
{
	#[OPcache\PinnedStatic]
	public static function value(): StaticAttributeCustomImmutableDate
	{
		static $object = null;

		$object ??= new StaticAttributeCustomImmutableDate('@0');
		return $object;
	}
}

class PinnedStaticPlainPropertyState
{
	#[OPcache\PinnedStatic]
	public static ?StaticAttributePlainObject $object = null;

	public static function value(): StaticAttributePlainObject
	{
		self::$object ??= new StaticAttributePlainObject();
		return self::$object;
	}
}

class PinnedStaticSafeDirectPropertyState
{
	#[OPcache\PinnedStatic]
	public static ?StaticAttributeCustomImmutableDate $object = null;

	public static function value(): StaticAttributeCustomImmutableDate
	{
		self::$object ??= new StaticAttributeCustomImmutableDate('@0');
		return self::$object;
	}
}

$action = $_GET['action'] ?? 'read';
$backend = $_GET['backend'] ?? 'volatile_static';
$kind = $_GET['kind'] ?? 'plain';

if ($action === 'reset') {
	OPcache\VolatileCache::clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$object = match ($backend . ':' . $kind) {
	'volatile_static:plain' => VolatileStaticPlainMethodState::value(),
	'volatile_static:safe_direct' => VolatileStaticSafeDirectMethodState::value(),
	'pinned_static:plain' => PinnedStaticPlainMethodState::value(),
	'pinned_static:safe_direct' => PinnedStaticSafeDirectMethodState::value(),
	'pinned_static_property:plain' => PinnedStaticPlainPropertyState::value(),
	'pinned_static_property:safe_direct' => PinnedStaticSafeDirectPropertyState::value(),
	default => throw new RuntimeException('unknown backend or kind'),
};

if ($action === 'mutate') {
	$object->bag[] = 'x';
}

echo $backend, '/', $kind, '=', count($object->bag), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_object_property_mutation_001.php';
foreach (['volatile_static', 'pinned_static', 'pinned_static_property'] as $backend) {
	foreach (['plain', 'safe_direct'] as $kind) {
		$query = 'backend=' . $backend . '&kind=' . $kind;
		echo file_get_contents($base . '?action=reset');
		echo file_get_contents($base . '?action=read&' . $query);
		echo file_get_contents($base . '?action=mutate&' . $query);
		echo file_get_contents($base . '?action=read&' . $query);
	}
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_object_property_mutation_001.php');
?>
--EXPECT--
reset
volatile_static/plain=0
volatile_static/plain=1
volatile_static/plain=1
reset
volatile_static/safe_direct=0
volatile_static/safe_direct=1
volatile_static/safe_direct=1
reset
pinned_static/plain=0
pinned_static/plain=1
pinned_static/plain=0
reset
pinned_static/safe_direct=0
pinned_static/safe_direct=1
pinned_static/safe_direct=0
reset
pinned_static_property/plain=0
pinned_static_property/plain=1
pinned_static_property/plain=0
reset
pinned_static_property/safe_direct=0
pinned_static_property/safe_direct=1
pinned_static_property/safe_direct=0

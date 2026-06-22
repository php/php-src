--TEST--
OPcache explicit cache delete rejects documented static attribute storage keys
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_attribute_exact_key_delete_001.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class ExactKeyStableClassState
{
	public static int $value = 0;

	public static function method(?int $set = null): int
	{
		static $value = 0;

		if ($set !== null) {
			$value = $set;
		}

		return $value;
	}
}

class ExactKeyStablePropertyState
{
	#[OPcache\StableStatic]
	public static int $value = 0;
}

class ExactKeyStableMethodState
{
	#[OPcache\StableStatic]
	public static function method(?int $set = null): int
	{
		static $value = 0;

		if ($set !== null) {
			$value = $set;
		}

		return $value;
	}
}

#[OPcache\VolatileStatic]
class ExactKeyVolatileClassState
{
	public static int $value = 0;

	public static function method(?int $set = null): int
	{
		static $value = 0;

		if ($set !== null) {
			$value = $set;
		}

		return $value;
	}
}

class ExactKeyVolatilePropertyState
{
	#[OPcache\VolatileStatic]
	public static int $value = 0;
}

class ExactKeyVolatileMethodState
{
	#[OPcache\VolatileStatic]
	public static function method(?int $set = null): int
	{
		static $value = 0;

		if ($set !== null) {
			$value = $set;
		}

		return $value;
	}
}

function exact_key_cache_info(string $backend): OPcache\StaticCacheInfo
{
	return match ($backend) {
		'stable' => OPcache\StableCache::info(),
		'volatile' => OPcache\VolatileCache::info(),
		default => throw new RuntimeException('unknown backend'),
	};
}

function exact_key_state(string $backend): array
{
	return match ($backend) {
		'stable' => [
			ExactKeyStableClassState::$value,
			ExactKeyStableClassState::method(),
			ExactKeyStablePropertyState::$value,
			ExactKeyStableMethodState::method(),
		],
		'volatile' => [
			ExactKeyVolatileClassState::$value,
			ExactKeyVolatileClassState::method(),
			ExactKeyVolatilePropertyState::$value,
			ExactKeyVolatileMethodState::method(),
		],
		default => throw new RuntimeException('unknown backend'),
	};
}

function exact_key_seed(string $backend): void
{
	switch ($backend) {
		case 'stable':
			ExactKeyStableClassState::$value = 1;
			ExactKeyStableClassState::method(1);
			ExactKeyStablePropertyState::$value = 1;
			ExactKeyStableMethodState::method(1);
			return;

		case 'volatile':
			ExactKeyVolatileClassState::$value = 1;
			ExactKeyVolatileClassState::method(1);
			ExactKeyVolatilePropertyState::$value = 1;
			ExactKeyVolatileMethodState::method(1);
			return;

		default:
			throw new RuntimeException('unknown backend');
	}
}

function exact_key_delete_individual(string $backend): string
{
	try {
		switch ($backend) {
			case 'stable':
				OPcache\StableCache::getInstance('default')->deleteMultiple([
					'stable_static:ExactKeyStablePropertyState::$value',
					'stable_static:ExactKeyStableMethodState::method()::$value',
				]);
				break;

			case 'volatile':
				OPcache\VolatileCache::getInstance('default')->deleteMultiple([
					'volatile_static:ExactKeyVolatilePropertyState::$value',
					'volatile_static:ExactKeyVolatileMethodState::method()::$value',
				]);
				break;

			default:
				throw new RuntimeException('unknown backend');
		}

		return 'deleted';
	} catch (ValueError $e) {
		return get_class($e) . ',delimiter=' . (str_contains($e->getMessage(), 'do not contain') ? '1' : '0');
	}
}

function exact_key_dump(string $label, string $backend): void
{
	echo $label, '-', $backend, '=',
		implode(',', exact_key_state($backend)),
		',count=', exact_key_cache_info($backend)->entry_count,
		"\n";
}

$action = $_GET['action'] ?? 'read';
$backend = $_GET['backend'] ?? 'stable';

if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	OPcache\StableCache::getInstance('default')->clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	exact_key_seed($backend);
	exact_key_dump('seed', $backend);
	return;
}

if ($action === 'delete_individual') {
	echo 'delete-individual-', $backend, '=',
		exact_key_delete_individual($backend),
		',count=', exact_key_cache_info($backend)->entry_count,
		"\n";
	return;
}

exact_key_dump('read', $backend);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_exact_key_delete_001.php';
foreach (['stable', 'volatile'] as $backend) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed&backend=' . $backend);
	echo file_get_contents($base . '?action=read&backend=' . $backend);
	echo file_get_contents($base . '?action=delete_individual&backend=' . $backend);
	echo file_get_contents($base . '?action=read&backend=' . $backend);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_attribute_exact_key_delete_001.php');
?>
--EXPECT--
reset
seed-stable=1,1,1,1,count=3
read-stable=1,1,1,1,count=3
delete-individual-stable=ValueError,delimiter=1,count=3
read-stable=1,1,1,1,count=3
reset
seed-volatile=1,1,1,1,count=3
read-volatile=1,1,1,1,count=3
delete-individual-volatile=ValueError,delimiter=1,count=3
read-volatile=1,1,1,1,count=3

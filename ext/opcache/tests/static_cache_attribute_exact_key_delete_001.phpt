--TEST--
OPcache static property and method attributes can be deleted by documented exact cache keys
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_attribute_exact_key_delete_001.php', <<<'PHP'
<?php
#[OPcache\PinnedStatic]
class ExactKeyPinnedClassState
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

class ExactKeyPinnedPropertyState
{
	#[OPcache\PinnedStatic]
	public static int $value = 0;
}

class ExactKeyPinnedMethodState
{
	#[OPcache\PinnedStatic]
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
		'pinned' => OPcache\PinnedCache::info(),
		'volatile' => OPcache\VolatileCache::info(),
		default => throw new RuntimeException('unknown backend'),
	};
}

function exact_key_state(string $backend): array
{
	return match ($backend) {
		'pinned' => [
			ExactKeyPinnedClassState::$value,
			ExactKeyPinnedClassState::method(),
			ExactKeyPinnedPropertyState::$value,
			ExactKeyPinnedMethodState::method(),
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
		case 'pinned':
			ExactKeyPinnedClassState::$value = 1;
			ExactKeyPinnedClassState::method(1);
			ExactKeyPinnedPropertyState::$value = 1;
			ExactKeyPinnedMethodState::method(1);
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

function exact_key_delete_individual(string $backend): void
{
	switch ($backend) {
		case 'pinned':
			OPcache\PinnedCache::deleteMultiple([
				'pinned_static:ExactKeyPinnedPropertyState::$value',
				'pinned_static:ExactKeyPinnedMethodState::method()::$value',
			]);
			return;

		case 'volatile':
			OPcache\VolatileCache::deleteMultiple([
				'volatile_static:ExactKeyVolatilePropertyState::$value',
				'volatile_static:ExactKeyVolatileMethodState::method()::$value',
			]);
			return;

		default:
			throw new RuntimeException('unknown backend');
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
$backend = $_GET['backend'] ?? 'pinned';

if ($action === 'reset') {
	OPcache\VolatileCache::clear();
	OPcache\PinnedCache::clear();
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
	exact_key_delete_individual($backend);
	echo 'delete-individual-', $backend, '=count=', exact_key_cache_info($backend)->entry_count, "\n";
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_exact_key_delete_001.php';
foreach (['pinned', 'volatile'] as $backend) {
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
seed-pinned=1,1,1,1,count=3
read-pinned=1,1,1,1,count=3
delete-individual-pinned=count=1
read-pinned=1,1,0,0,count=1
reset
seed-volatile=1,1,1,1,count=3
read-volatile=1,1,1,1,count=3
delete-individual-volatile=count=1
read-volatile=1,1,0,0,count=1

--TEST--
OPcache explicit cache delete rejects loaded class names and leaves static attributes intact
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_attribute_class_delete_001.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class ClassDeleteStableBlob
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

class ClassDeleteStableMembers
{
	#[OPcache\StableStatic]
	public static int $property = 0;

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
class ClassDeleteVolatileBlob
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

class ClassDeleteVolatileMembers
{
	#[OPcache\VolatileStatic]
	public static int $property = 0;

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

class ClassDeleteStablePlain
{
}

class ClassDeleteVolatilePlain
{
}

function class_delete_cache_info(string $backend): OPcache\StaticCacheInfo
{
	return match ($backend) {
		'stable' => OPcache\StableCache::info(),
		'volatile' => OPcache\VolatileCache::info(),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_store(string $backend, string $key, mixed $value): void
{
	match ($backend) {
		'stable' => OPcache\StableCache::getInstance('default')->store($key, $value),
		'volatile' => OPcache\VolatileCache::getInstance('default')->store($key, $value),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_fetch(string $backend, string $key, mixed $default): mixed
{
	return match ($backend) {
		'stable' => OPcache\StableCache::getInstance('default')->fetch($key, $default),
		'volatile' => OPcache\VolatileCache::getInstance('default')->fetch($key, $default),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_delete(string $backend, string $key_or_class): void
{
	match ($backend) {
		'stable' => OPcache\StableCache::getInstance('default')->delete($key_or_class),
		'volatile' => OPcache\VolatileCache::getInstance('default')->delete($key_or_class),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_delete_result(string $backend, string $key_or_class): string
{
	try {
		class_delete_delete($backend, $key_or_class);

		return 'deleted';
	} catch (ValueError $e) {
		return get_class($e) . ',loaded=' . (str_contains($e->getMessage(), 'loaded class name') ? '1' : '0');
	}
}

function class_delete_blob_class(string $backend): string
{
	return match ($backend) {
		'stable' => ClassDeleteStableBlob::class,
		'volatile' => ClassDeleteVolatileBlob::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_members_class(string $backend): string
{
	return match ($backend) {
		'stable' => ClassDeleteStableMembers::class,
		'volatile' => ClassDeleteVolatileMembers::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_plain_class(string $backend): string
{
	return match ($backend) {
		'stable' => ClassDeleteStablePlain::class,
		'volatile' => ClassDeleteVolatilePlain::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_blob_state(string $backend): array
{
	return match ($backend) {
		'stable' => [
			ClassDeleteStableBlob::$value,
			ClassDeleteStableBlob::method(),
		],
		'volatile' => [
			ClassDeleteVolatileBlob::$value,
			ClassDeleteVolatileBlob::method(),
		],
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_members_state(string $backend): array
{
	return match ($backend) {
		'stable' => [
			ClassDeleteStableMembers::$property,
			ClassDeleteStableMembers::method(),
		],
		'volatile' => [
			ClassDeleteVolatileMembers::$property,
			ClassDeleteVolatileMembers::method(),
		],
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_seed_blob(string $backend): void
{
	switch ($backend) {
		case 'stable':
			ClassDeleteStableBlob::$value = 1;
			ClassDeleteStableBlob::method(1);
			break;

		case 'volatile':
			ClassDeleteVolatileBlob::$value = 1;
			ClassDeleteVolatileBlob::method(1);
			break;

		default:
			throw new RuntimeException('unknown backend');
	}
}

function class_delete_seed_members(string $backend): void
{
	switch ($backend) {
		case 'stable':
			ClassDeleteStableMembers::$property = 1;
			ClassDeleteStableMembers::method(1);
			break;

		case 'volatile':
			ClassDeleteVolatileMembers::$property = 1;
			ClassDeleteVolatileMembers::method(1);
			break;

		default:
			throw new RuntimeException('unknown backend');
	}
}

function class_delete_dump_blob(string $label, string $backend): void
{
	echo $label, '-', $backend, '=',
		implode(',', class_delete_blob_state($backend)),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
}

function class_delete_dump_members(string $label, string $backend): void
{
	echo $label, '-', $backend, '=',
		implode(',', class_delete_members_state($backend)),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
}

$action = $_GET['action'] ?? 'read_blob';
$backend = $_GET['backend'] ?? 'stable';

if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	OPcache\StableCache::getInstance('default')->clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed_blob') {
	class_delete_seed_blob($backend);
	class_delete_dump_blob('seed-blob', $backend);
	return;
}

if ($action === 'read_blob') {
	class_delete_dump_blob('read-blob', $backend);
	return;
}

if ($action === 'delete_blob') {
	$key_or_class = class_delete_blob_class($backend);
	if ($backend === 'volatile') {
		$key_or_class = '\\' . $key_or_class;
	}
	echo 'delete-blob-', $backend, '=',
		class_delete_delete_result($backend, $key_or_class),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
	return;
}

if ($action === 'seed_members') {
	class_delete_seed_members($backend);
	class_delete_dump_members('seed-members', $backend);
	return;
}

if ($action === 'read_members') {
	class_delete_dump_members('read-members', $backend);
	return;
}

if ($action === 'delete_members') {
	echo 'delete-members-', $backend, '=',
		class_delete_delete_result($backend, class_delete_members_class($backend)),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
	return;
}

if ($action === 'delete_plain') {
	echo 'delete-plain-', $backend, '=',
		class_delete_delete_result($backend, class_delete_plain_class($backend)),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
	return;
}

if ($action === 'noautoload') {
	$key = 'ClassDelete\\MissingAutoload';
	class_delete_store($backend, $key, 'autoload');
	spl_autoload_register(static function (string $class): void {
		echo 'autoload-', $class, "\n";
	});
	class_delete_delete($backend, $key);
	echo 'noautoload-', $backend, '=',
		class_delete_fetch($backend, $key, 'missing'),
		',count=', class_delete_cache_info($backend)->entry_count,
		"\n";
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_class_delete_001.php';
foreach (['stable', 'volatile'] as $backend) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed_blob&backend=' . $backend);
	echo file_get_contents($base . '?action=read_blob&backend=' . $backend);
	echo file_get_contents($base . '?action=delete_blob&backend=' . $backend);
	echo file_get_contents($base . '?action=read_blob&backend=' . $backend);
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed_members&backend=' . $backend);
	echo file_get_contents($base . '?action=read_members&backend=' . $backend);
	echo file_get_contents($base . '?action=delete_members&backend=' . $backend);
	echo file_get_contents($base . '?action=read_members&backend=' . $backend);
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=delete_plain&backend=' . $backend);
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=noautoload&backend=' . $backend);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_attribute_class_delete_001.php');
?>
--EXPECT--
reset
seed-blob-stable=1,1,count=1
read-blob-stable=1,1,count=1
delete-blob-stable=ValueError,loaded=1,count=1
read-blob-stable=1,1,count=1
reset
seed-members-stable=1,1,count=2
read-members-stable=1,1,count=2
delete-members-stable=ValueError,loaded=1,count=2
read-members-stable=1,1,count=2
reset
delete-plain-stable=ValueError,loaded=1,count=0
reset
noautoload-stable=missing,count=0
reset
seed-blob-volatile=1,1,count=1
read-blob-volatile=1,1,count=1
delete-blob-volatile=ValueError,loaded=1,count=1
read-blob-volatile=1,1,count=1
reset
seed-members-volatile=1,1,count=2
read-members-volatile=1,1,count=2
delete-members-volatile=ValueError,loaded=1,count=2
read-members-volatile=1,1,count=2
reset
delete-plain-volatile=ValueError,loaded=1,count=0
reset
noautoload-volatile=missing,count=0

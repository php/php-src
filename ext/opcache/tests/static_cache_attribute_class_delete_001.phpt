--TEST--
OPcache static attributes can be deleted by their class names
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_attribute_class_delete_001.php', <<<'PHP'
<?php
#[OPcache\PinnedStatic]
class ClassDeletePinnedBlob
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

class ClassDeletePinnedMembers
{
	#[OPcache\PinnedStatic]
	public static int $property = 0;

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

class ClassDeletePinnedPlain
{
}

class ClassDeleteVolatilePlain
{
}

function class_delete_cache_info(string $backend): OPcache\StaticCacheInfo
{
	return match ($backend) {
		'pinned' => OPcache\pinned_cache_info(),
		'volatile' => OPcache\volatile_cache_info(),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_store(string $backend, string $key, mixed $value): void
{
	match ($backend) {
		'pinned' => OPcache\pinned_store($key, $value),
		'volatile' => OPcache\volatile_store($key, $value),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_fetch(string $backend, string $key, mixed $default): mixed
{
	return match ($backend) {
		'pinned' => OPcache\pinned_fetch($key, $default),
		'volatile' => OPcache\volatile_fetch($key, $default),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_delete(string $backend, string $key_or_class): void
{
	match ($backend) {
		'pinned' => OPcache\pinned_delete($key_or_class),
		'volatile' => OPcache\volatile_delete($key_or_class),
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_blob_class(string $backend): string
{
	return match ($backend) {
		'pinned' => ClassDeletePinnedBlob::class,
		'volatile' => ClassDeleteVolatileBlob::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_members_class(string $backend): string
{
	return match ($backend) {
		'pinned' => ClassDeletePinnedMembers::class,
		'volatile' => ClassDeleteVolatileMembers::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_plain_class(string $backend): string
{
	return match ($backend) {
		'pinned' => ClassDeletePinnedPlain::class,
		'volatile' => ClassDeleteVolatilePlain::class,
		default => throw new RuntimeException('unknown backend'),
	};
}

function class_delete_blob_state(string $backend): array
{
	return match ($backend) {
		'pinned' => [
			ClassDeletePinnedBlob::$value,
			ClassDeletePinnedBlob::method(),
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
		'pinned' => [
			ClassDeletePinnedMembers::$property,
			ClassDeletePinnedMembers::method(),
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
		case 'pinned':
			ClassDeletePinnedBlob::$value = 1;
			ClassDeletePinnedBlob::method(1);
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
		case 'pinned':
			ClassDeletePinnedMembers::$property = 1;
			ClassDeletePinnedMembers::method(1);
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
$backend = $_GET['backend'] ?? 'pinned';

if ($action === 'reset') {
	OPcache\volatile_clear();
	OPcache\pinned_clear();
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
	class_delete_delete($backend, $key_or_class);
	echo 'delete-blob-', $backend, '=count=', class_delete_cache_info($backend)->entry_count, "\n";
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
	class_delete_delete($backend, class_delete_members_class($backend));
	echo 'delete-members-', $backend, '=count=', class_delete_cache_info($backend)->entry_count, "\n";
	return;
}

if ($action === 'delete_plain') {
	class_delete_delete($backend, class_delete_plain_class($backend));
	echo 'delete-plain-', $backend, '=count=', class_delete_cache_info($backend)->entry_count, "\n";
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_attribute_class_delete_001.php';
foreach (['pinned', 'volatile'] as $backend) {
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
seed-blob-pinned=1,1,count=1
read-blob-pinned=1,1,count=1
delete-blob-pinned=count=0
read-blob-pinned=0,0,count=0
reset
seed-members-pinned=1,1,count=2
read-members-pinned=1,1,count=2
delete-members-pinned=count=0
read-members-pinned=0,0,count=0
reset
delete-plain-pinned=count=0
reset
noautoload-pinned=missing,count=0
reset
seed-blob-volatile=1,1,count=1
read-blob-volatile=1,1,count=1
delete-blob-volatile=count=0
read-blob-volatile=0,0,count=0
reset
seed-members-volatile=1,1,count=2
read-members-volatile=1,1,count=2
delete-members-volatile=count=0
read-members-volatile=0,0,count=0
reset
delete-plain-volatile=count=0
reset
noautoload-volatile=missing,count=0

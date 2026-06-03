--TEST--
OPcache static attribute decode capture is cleared after cache miss
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_capture_miss_001.php', <<<'PHP'
<?php

class CaptureMissProbe
{
	public array $bag = [];
}

class CaptureMissCachedMethod
{
	#[OPcache\VolatileStatic]
	public static function touch(): void
	{
		static $value = null;
	}
}

class CaptureMissStableMethod
{
	#[OPcache\StableStatic]
	public static function touch(): void
	{
		static $value = null;
	}
}

$action = $_GET['action'] ?? 'seed';
$backend = $_GET['backend'] ?? 'cached';

if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	$probe = new CaptureMissProbe();
	$probe->bag[] = 'seed';
	var_dump(OPcache\VolatileCache::getInstance('default')->store('capture_miss_explicit', $probe));
	echo 'volatile_entries=', OPcache\VolatileCache::info()->entry_count, "\n";
	return;
}

if ($action === 'miss_then_explicit_mutate') {
	match ($backend) {
		'cached' => CaptureMissCachedMethod::touch(),
		'stable' => CaptureMissStableMethod::touch(),
		default => throw new RuntimeException('unknown backend'),
	};

	$explicit = OPcache\VolatileCache::getInstance('default')->fetch('capture_miss_explicit');
	$explicit->bag[] = 'mutated outside static attribute';

	echo 'explicit_bag=', count($explicit->bag), "\n";
	echo 'volatile_entries=', OPcache\VolatileCache::info()->entry_count, "\n";
	echo 'stable_entries=', OPcache\StableCache::info()->entry_count, "\n";
	return;
}

if ($action === 'read_static') {
	match ($backend) {
		'cached' => CaptureMissCachedMethod::touch(),
		'stable' => CaptureMissStableMethod::touch(),
		default => throw new RuntimeException('unknown backend'),
	};
	echo 'volatile_entries=', OPcache\VolatileCache::info()->entry_count, "\n";
	echo 'stable_entries=', OPcache\StableCache::info()->entry_count, "\n";
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_capture_miss_001.php';
foreach (['cached', 'stable'] as $backend) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=seed');
	echo file_get_contents($base . '?action=miss_then_explicit_mutate&backend=' . $backend);
	echo file_get_contents($base . '?action=read_static&backend=' . $backend);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_capture_miss_001.php');
?>
--EXPECT--
reset
bool(true)
volatile_entries=1
explicit_bag=2
volatile_entries=1
stable_entries=0
volatile_entries=1
stable_entries=0
reset
bool(true)
volatile_entries=1
explicit_bag=2
volatile_entries=1
stable_entries=0
volatile_entries=1
stable_entries=0

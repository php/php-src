--TEST--
OPcache static attributes handle ArrayObject dimension writes by publication kind
--EXTENSIONS--
opcache
spl
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_object_dim_mutation_001.php', <<<'PHP'
<?php
class VolatileStaticArrayObjectMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): ArrayObject
	{
		static $object = null;

		$object ??= new ArrayObject(['count' => 0, 'events' => [], 'remove' => 'seed']);
		return $object;
	}
}

class PinnedStaticArrayObjectMethodState
{
	#[OPcache\PinnedStatic]
	public static function value(): ArrayObject
	{
		static $object = null;

		$object ??= new ArrayObject(['count' => 0, 'events' => [], 'remove' => 'seed']);
		return $object;
	}
}

class PinnedStaticArrayObjectPropertyState
{
	#[OPcache\PinnedStatic]
	public static ?ArrayObject $object = null;

	public static function value(): ArrayObject
	{
		self::$object ??= new ArrayObject(['count' => 0, 'events' => [], 'remove' => 'seed']);
		return self::$object;
	}
}

$action = $_GET['action'] ?? 'read';
$backend = $_GET['backend'] ?? 'volatile_static';

if ($action === 'reset') {
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$object = match ($backend) {
	'volatile_static' => VolatileStaticArrayObjectMethodState::value(),
	'pinned_static' => PinnedStaticArrayObjectMethodState::value(),
	'pinned_static_property' => PinnedStaticArrayObjectPropertyState::value(),
	default => throw new RuntimeException('unknown backend'),
};

if ($action === 'mutate') {
	$object['count'] += 1;
	$object['events'] = ['x'];
	unset($object['remove']);
}

echo $backend, '=', (int) $object['count'], ',', count($object['events']), ',', ($object->offsetExists('remove') ? 1 : 0), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_object_dim_mutation_001.php';
foreach (['volatile_static', 'pinned_static', 'pinned_static_property'] as $backend) {
	$query = 'backend=' . $backend;
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?action=read&' . $query);
	echo file_get_contents($base . '?action=mutate&' . $query);
	echo file_get_contents($base . '?action=read&' . $query);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_object_dim_mutation_001.php');
?>
--EXPECT--
reset
volatile_static=0,0,1
volatile_static=1,1,0
volatile_static=1,1,0
reset
pinned_static=0,0,1
pinned_static=1,1,0
pinned_static=0,0,1
reset
pinned_static_property=0,0,1
pinned_static_property=1,1,0
pinned_static_property=0,0,1

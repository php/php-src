--TEST--
OPcache VolatileStatic TTL expires class, property, and method state
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_ttl_001.php', <<<'PHP'
<?php
#[OPcache\VolatileStatic(ttl: 1)]
class VolatileStaticTtlImmediateClass
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class VolatileStaticTtlImmediateProperty
{
	#[OPcache\VolatileStatic(ttl: 1)]
	public static int $value = 0;
}

class VolatileStaticTtlImmediateMethod
{
	#[OPcache\VolatileStatic(ttl: 1)]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

#[OPcache\VolatileStatic(ttl: 1, strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTtlTrackingClass
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class VolatileStaticTtlTrackingProperty
{
	#[OPcache\VolatileStatic(ttl: 1, strategy: OPcache\CacheStrategy::Tracking)]
	public static int $value = 0;
}

class VolatileStaticTtlTrackingMethod
{
	#[OPcache\VolatileStatic(ttl: 1, strategy: OPcache\CacheStrategy::Tracking)]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$action = $_GET['action'] ?? 'step';

if ($action === 'reset') {
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$values = [
	++VolatileStaticTtlImmediateClass::$value,
	VolatileStaticTtlImmediateClass::next(),
	++VolatileStaticTtlImmediateProperty::$value,
	VolatileStaticTtlImmediateMethod::next(),
	++VolatileStaticTtlTrackingClass::$value,
	VolatileStaticTtlTrackingClass::next(),
	++VolatileStaticTtlTrackingProperty::$value,
	VolatileStaticTtlTrackingMethod::next(),
];

echo $action, '=', implode(',', $values), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_ttl_001.php';

echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=first');
echo file_get_contents($base . '?action=second');
sleep(2);
echo file_get_contents($base . '?action=expired');
echo file_get_contents($base . '?action=again');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_ttl_001.php');
?>
--EXPECT--
reset
first=1,1,1,1,1,1,1,1
second=2,2,2,2,2,2,2,2
expired=1,1,1,1,1,1,1,1
again=2,2,2,2,2,2,2,2

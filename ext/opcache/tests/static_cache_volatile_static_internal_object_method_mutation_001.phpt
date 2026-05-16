--TEST--
OPcache VolatileStatic tracks internal object method mutations
--EXTENSIONS--
opcache
spl
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_internal_object_method_mutation_001.php', <<<'PHP'
<?php

class VolatileStaticDateMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): DateTime
	{
		static $value = null;

		$value ??= new DateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'));
		return $value;
	}
}

class VolatileStaticArrayObjectMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): ArrayObject
	{
		static $value = null;

		$value ??= new ArrayObject(['count' => 0]);
		return $value;
	}
}

class VolatileStaticFixedArrayMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): SplFixedArray
	{
		static $value = null;

		$value ??= SplFixedArray::fromArray(['seed']);
		return $value;
	}
}

$action = $_GET['action'] ?? 'read';
$state = $_GET['state'] ?? 'date';

if ($action === 'reset') {
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($state === 'date') {
	$value = VolatileStaticDateMethodState::value();
	if ($action === 'mutate') {
		$value->modify('+1 day');
	}
	echo "date=", $value->format('Y-m-d'), "\n";
	return;
}

if ($state === 'array') {
	$value = VolatileStaticArrayObjectMethodState::value();
	if ($action === 'mutate') {
		$value->offsetSet('count', $value->offsetGet('count') + 1);
		$value->append('tail');
	}
	echo "array=", $value->offsetGet('count'), ",", count($value), "\n";
	return;
}

$value = VolatileStaticFixedArrayMethodState::value();
if ($action === 'mutate') {
	$value->offsetSet(0, 'changed');
	$value->setSize(2);
	$value->offsetSet(1, 'tail');
}
$second = $value->count() > 1 ? $value->offsetGet(1) : 'none';
echo "fixed=", $value->count(), ",", $value->offsetGet(0), ",", $second, "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_internal_object_method_mutation_001.php';
foreach (['date', 'array', 'fixed'] as $state) {
	echo file_get_contents($base . '?action=reset');
	echo file_get_contents($base . '?state=' . $state . '&action=read');
	echo file_get_contents($base . '?state=' . $state . '&action=mutate');
	echo file_get_contents($base . '?state=' . $state . '&action=read');
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_internal_object_method_mutation_001.php');
?>
--EXPECT--
reset
date=2026-01-01
date=2026-01-02
date=2026-01-02
reset
array=0,1
array=1,2
array=1,2
reset
fixed=1,seed,none
fixed=2,changed,tail
fixed=2,changed,tail

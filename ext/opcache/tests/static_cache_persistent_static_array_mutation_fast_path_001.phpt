--TEST--
OPcache StableStatic publishes class, property, and method array mutations immediately
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_stable_static_array_mutation_fast_path_001.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class StableStaticArrayClassState
{
	public static array $routes = [];
}

class StableStaticArrayPropertyState
{
	#[OPcache\StableStatic]
	public static array $routes = [];
}

class StableStaticArrayMethodState
{
	#[OPcache\StableStatic]
	public static function routes(?string $append = null): array
	{
		static $routes = [];

		if ($append !== null) {
			$routes[] = $append;
		}

		return $routes;
	}
}

function dump_routes(string $label): void
{
	echo $label, '-class=', implode(',', StableStaticArrayClassState::$routes), "\n";
	echo $label, '-property=', implode(',', StableStaticArrayPropertyState::$routes), "\n";
	echo $label, '-method=', implode(',', StableStaticArrayMethodState::routes()), "\n";
}

$action = $_GET['action'] ?? 'read';

if ($action === 'reset') {
	OPcache\StableCache::getInstance('default')->clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	StableStaticArrayClassState::$routes = ['foo'];
	StableStaticArrayClassState::$routes[] = 'bar';
	StableStaticArrayPropertyState::$routes = ['foo'];
	StableStaticArrayPropertyState::$routes[] = 'bar';
	StableStaticArrayMethodState::routes('foo');
	StableStaticArrayMethodState::routes('bar');
	dump_routes('seed');
	return;
}

if ($action === 'mutate') {
	StableStaticArrayClassState::$routes[] = 'baz';
	StableStaticArrayPropertyState::$routes[] = 'baz';
	StableStaticArrayMethodState::routes('baz');
	dump_routes('mutate');
	return;
}

dump_routes('read');
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_stable_static_array_mutation_fast_path_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=read');
echo file_get_contents($base . '?action=mutate');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_stable_static_array_mutation_fast_path_001.php');
?>
--EXPECT--
reset
seed-class=foo,bar
seed-property=foo,bar
seed-method=foo,bar
read-class=foo,bar
read-property=foo,bar
read-method=foo,bar
mutate-class=foo,bar,baz
mutate-property=foo,bar,baz
mutate-method=foo,bar,baz
read-class=foo,bar,baz
read-property=foo,bar,baz
read-method=foo,bar,baz

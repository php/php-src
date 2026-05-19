--TEST--
OPcache PinnedStatic publishes class, property, and method array mutations immediately
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_pinned_static_array_mutation_fast_path_001.php', <<<'PHP'
<?php
#[OPcache\PinnedStatic]
class PinnedStaticArrayClassState
{
	public static array $routes = [];
}

class PinnedStaticArrayPropertyState
{
	#[OPcache\PinnedStatic]
	public static array $routes = [];
}

class PinnedStaticArrayMethodState
{
	#[OPcache\PinnedStatic]
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
	echo $label, '-class=', implode(',', PinnedStaticArrayClassState::$routes), "\n";
	echo $label, '-property=', implode(',', PinnedStaticArrayPropertyState::$routes), "\n";
	echo $label, '-method=', implode(',', PinnedStaticArrayMethodState::routes()), "\n";
}

$action = $_GET['action'] ?? 'read';

if ($action === 'reset') {
	OPcache\pinned_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	PinnedStaticArrayClassState::$routes = ['foo'];
	PinnedStaticArrayClassState::$routes[] = 'bar';
	PinnedStaticArrayPropertyState::$routes = ['foo'];
	PinnedStaticArrayPropertyState::$routes[] = 'bar';
	PinnedStaticArrayMethodState::routes('foo');
	PinnedStaticArrayMethodState::routes('bar');
	dump_routes('seed');
	return;
}

if ($action === 'mutate') {
	PinnedStaticArrayClassState::$routes[] = 'baz';
	PinnedStaticArrayPropertyState::$routes[] = 'baz';
	PinnedStaticArrayMethodState::routes('baz');
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_pinned_static_array_mutation_fast_path_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=read');
echo file_get_contents($base . '?action=mutate');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_pinned_static_array_mutation_fast_path_001.php');
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

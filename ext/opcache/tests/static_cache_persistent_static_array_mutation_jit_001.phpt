--TEST--
OPcache PinnedStatic array mutations remain visible through tracing JIT static slot reads
--EXTENSIONS--
opcache
--CONFLICTS--
server
--SKIPIF--
<?php
if (!array_key_exists('opcache.jit', opcache_get_configuration()['directives'])) {
	die('skip no JIT support');
}
?>
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_array_mutation_jit_001.php', <<<'PHP'
<?php
function pinned_static_jit_on(): int
{
	return (int) (opcache_get_status()['jit']['on'] ?? false);
}

class JitPinnedStaticSlotPropertyState
{
	#[OPcache\PinnedStatic]
	public static array $routes = [];
}

class JitPinnedStaticSlotMethodState
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

function jit_property_count(): int
{
	$total = 0;

	for ($i = 0; $i < 1000; $i++) {
		$total += count(JitPinnedStaticSlotPropertyState::$routes);
	}

	return $total;
}

function jit_method_count(): int
{
	$total = 0;

	for ($i = 0; $i < 1000; $i++) {
		$total += count(JitPinnedStaticSlotMethodState::routes());
	}

	return $total;
}

function warm_jit(): void
{
	for ($i = 0; $i < 20; $i++) {
		jit_property_count();
		jit_method_count();
	}
}

$action = $_GET['action'] ?? 'read';

if ($action === 'reset') {
	OPcache\pinned_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	JitPinnedStaticSlotPropertyState::$routes = ['foo'];
	JitPinnedStaticSlotMethodState::routes('foo');
	warm_jit();
	echo 'seed-before=', pinned_static_jit_on(), ',', jit_property_count(), ',', jit_method_count(), "\n";
	JitPinnedStaticSlotPropertyState::$routes[] = 'bar';
	JitPinnedStaticSlotMethodState::routes('bar');
	echo 'seed-after=', pinned_static_jit_on(), ',', jit_property_count(), ',', jit_method_count(), "\n";
	return;
}

warm_jit();
echo 'read=', pinned_static_jit_on(), ',', jit_property_count(), ',', jit_method_count(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=tracing -d opcache.jit_buffer_size=64M -d opcache.jit_hot_loop=1 -d opcache.jit_hot_func=1 -d opcache.jit_hot_return=1 -d opcache.jit_hot_side_exit=1');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_array_mutation_jit_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_array_mutation_jit_001.php');
?>
--EXPECT--
reset
seed-before=1,1000,1000
seed-after=1,2000,2000
read=1,2000,2000

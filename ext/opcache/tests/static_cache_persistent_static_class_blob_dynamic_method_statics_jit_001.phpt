--TEST--
OPcache PinnedStatic class blob survives dynamic method statics with JIT enabled
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

file_put_contents(__DIR__ . '/pinned_static_007.php', <<<'PHP'
<?php
function pinned_static_seed(string $logFile): int
{
	file_put_contents($logFile, "seed\n", FILE_APPEND);
	return 1;
}

function pinned_static_jit_on(): int
{
	return (int) (opcache_get_status()['jit']['on'] ?? false);
}

#[OPcache\PinnedStatic]
class JitCombinedBlobState
{
	public static int $count = 0;
	public static array $bag = [];

	public static function nextValue(string $logFile): int
	{
		static $value = pinned_static_seed($logFile);

		return ++$value;
	}
}

$request = (int) ($_GET['request'] ?? 1);
$logFile = __DIR__ . '/pinned_static_007.log';

if ($request === 1) {
	$last = 0;

	for ($i = 0; $i < 6; $i++) {
		$last = JitCombinedBlobState::nextValue($logFile);
		JitCombinedBlobState::$count++;
	}

	JitCombinedBlobState::$bag['numbers'][] = 10;
	JitCombinedBlobState::$bag['numbers'][] = 11;

	echo pinned_static_jit_on(), ',', $last, ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";
	return;
}

if ($request === 2) {
	echo pinned_static_jit_on(), ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', OPcache\pinned_cache_info()->entry_count, ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";

	$last = 0;
	for ($i = 0; $i < 4; $i++) {
		$last = JitCombinedBlobState::nextValue($logFile);
	}
	JitCombinedBlobState::$count++;
	JitCombinedBlobState::$bag['numbers'][] = 12;

	echo $last, ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";
	return;
}

echo pinned_static_jit_on(), ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', OPcache\pinned_cache_info()->entry_count, ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

@unlink(__DIR__ . '/pinned_static_007.log');

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=tracing -d opcache.jit_buffer_size=64M -d opcache.jit_hot_loop=0 -d opcache.jit_hot_func=2 -d opcache.jit_hot_return=0 -d opcache.jit_hot_side_exit=1');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_007.php?request=1');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_007.php?request=2');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_007.php?request=3');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_007.php');
@unlink(__DIR__ . '/pinned_static_007.log');
?>
--EXPECT--
1,7,6,21,1
1,6,21,1,1
11,7,33,1
1,7,33,1,1

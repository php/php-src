--TEST--
FPM: OPcache PersistentStatic class blob survives dynamic method statics with JIT enabled across requests
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
if (!array_key_exists('opcache.jit', opcache_get_configuration()['directives'])) {
	die('skip no JIT support');
}
?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[opcache]
listen = {{ADDR}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php
function persistent_static_seed(string $logFile): int
{
	file_put_contents($logFile, "seed\n", FILE_APPEND);
	return 1;
}

function persistent_static_jit_on(): int
{
	return (int) (opcache_get_status()['jit']['on'] ?? false);
}

#[OPcache\PersistentStatic]
class JitCombinedBlobState
{
	public static int $count = 0;
	public static array $bag = [];

	public static function nextValue(string $logFile): int
	{
		static $value = persistent_static_seed($logFile);

		return ++$value;
	}
}

$request = (int) ($_GET['request'] ?? 1);
$logFile = __DIR__ . '/persistent_static_fpm_006.log';

if ($request === 1) {
	@unlink($logFile);
	$last = 0;

	for ($i = 0; $i < 6; $i++) {
		$last = JitCombinedBlobState::nextValue($logFile);
		JitCombinedBlobState::$count++;
	}

	JitCombinedBlobState::$bag['numbers'][] = 10;
	JitCombinedBlobState::$bag['numbers'][] = 11;

	echo persistent_static_jit_on(), ',', $last, ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', count(file($logFile, FILE_IGNORE_NEW_LINES));
	return;
}

if ($request === 2) {
	echo persistent_static_jit_on(), ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', OPcache\persistent_cache_info()->entry_count, ',', count(file($logFile, FILE_IGNORE_NEW_LINES)), "\n";

	$last = 0;
	for ($i = 0; $i < 4; $i++) {
		$last = JitCombinedBlobState::nextValue($logFile);
	}
	JitCombinedBlobState::$count++;
	JitCombinedBlobState::$bag['numbers'][] = 12;

	echo $last, ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', count(file($logFile, FILE_IGNORE_NEW_LINES));
	return;
}

echo persistent_static_jit_on(), ',', JitCombinedBlobState::$count, ',', array_sum(JitCombinedBlobState::$bag['numbers']), ',', OPcache\persistent_cache_info()->entry_count, ',', count(file($logFile, FILE_IGNORE_NEW_LINES));
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.persistent_size_mb' => '32',
	'opcache.file_update_protection' => '0',
	'opcache.jit' => 'tracing',
	'opcache.jit_buffer_size' => '64M',
	'opcache.jit_hot_loop' => '0',
	'opcache.jit_hot_func' => '2',
	'opcache.jit_hot_return' => '0',
	'opcache.jit_hot_side_exit' => '1',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'request=1')->expectBody('1,7,6,21,1');
$tester->request(query: 'request=2')->expectBody("1,6,21,1,1\n11,7,33,1");
$tester->request(query: 'request=3')->expectBody('1,7,33,1,1');

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();

echo "Done\n";

?>
--EXPECT--
Done
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
@unlink(__DIR__ . '/persistent_static_fpm_006.log');
?>

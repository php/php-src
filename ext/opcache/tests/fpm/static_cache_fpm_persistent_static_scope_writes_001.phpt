--TEST--
FPM: OPcache PinnedStatic class, property, and method state handles sequential writes by scope
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
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
#[OPcache\PinnedStatic]
class FpmRaceClassState
{
	public static array $data = ['value' => 0, 'numbers' => []];

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class FpmRacePropertyState
{
	#[OPcache\PinnedStatic]
	public static array $data = ['value' => 0, 'numbers' => []];
}

class FpmRaceMethodState
{
	#[OPcache\PinnedStatic]
	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$action = $_GET['action'] ?? 'reset';

if ($action === 'reset') {
	opcache_reset();
	echo 'reset';
	return;
}

if ($action === 'seed') {
	FpmRaceClassState::$data = ['value' => 1, 'numbers' => [1]];
	FpmRaceClassState::next();
	FpmRacePropertyState::$data = ['value' => 1, 'numbers' => [1]];
	FpmRaceMethodState::next();
	echo 'seed';
	return;
}

$tuple = static function (): string {
	return FpmRaceClassState::$data['value'] . ','
		. array_sum(FpmRaceClassState::$data['numbers']) . ','
		. FpmRaceClassState::next() . ','
		. FpmRacePropertyState::$data['value'] . ','
		. array_sum(FpmRacePropertyState::$data['numbers']) . ','
		. FpmRaceMethodState::next();
};

if ($action === 'writer') {
	$classMethod = FpmRaceClassState::next();
	FpmRaceClassState::$data['value'] = 10;
	FpmRaceClassState::$data['numbers'][] = 20;
	FpmRacePropertyState::$data['value'] = 10;
	FpmRacePropertyState::$data['numbers'][] = 20;
	$method = FpmRaceMethodState::next();
	echo 'writer:', FpmRaceClassState::$data['value'], ',', array_sum(FpmRaceClassState::$data['numbers']), ',', $classMethod, ',', FpmRacePropertyState::$data['value'], ',', array_sum(FpmRacePropertyState::$data['numbers']), ',', $method;
	return;
}

if ($action === 'verify') {
	echo 'verify:', $tuple();
	return;
}
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.pinned_size_mb' => '32',
	'opcache.file_update_protection' => '0',
]);
$tester->expectLogStartNotices();

$tester->request(query: 'action=reset')->expectBody('reset');
$tester->request(query: 'action=seed')->expectBody('seed');
$tester->request(query: 'action=writer')->expectBody('writer:10,21,2,10,21,2');
$tester->request(query: 'action=verify')->expectBody('verify:10,21,3,10,21,3');

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
?>

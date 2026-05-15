--TEST--
FPM: OPcache PersistentStatic persists complex values across requests
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
class CounterBox
{
	public function __construct(public int $value) {}
}

#[OPcache\PersistentStatic]
class ComplexState
{
	public static ?CounterBox $box = null;
	public static array $gap = [];

	public static function methodState(): string
	{
		static $state = null;

		if ($state === null) {
			$state = ['box' => new CounterBox(100), 'gap' => []];
			$state['gap'][4] = 'seed';
			unset($state['gap'][4]);
		}

		$state['box']->value++;
		$state['gap'][] = 'tail';

		return $state['box']->value . ':' . array_key_last($state['gap']);
	}
}

if (ComplexState::$box === null) {
	ComplexState::$box = new CounterBox(1);
	ComplexState::$gap[3] = 'seed';
	unset(ComplexState::$gap[3]);
}

ComplexState::$box->value++;
ComplexState::$gap[] = 'tail';

echo ComplexState::$box->value, ',', array_key_last(ComplexState::$gap), ',', ComplexState::methodState(), "\n";
PHP;

$tester = new FPM\Tester($cfg, $code);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.persistent_size_mb' => '32',
]);
$tester->expectLogStartNotices();

$tester->request()->expectBody('2,4,101:5');
$tester->request()->expectBody('3,5,102:6');

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

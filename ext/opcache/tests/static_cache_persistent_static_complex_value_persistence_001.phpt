--TEST--
OPcache PinnedStatic persists complex static values across requests
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_003.php', <<<'PHP'
<?php
class CounterBox
{
	public function __construct(public int $value) {}
}

#[OPcache\PinnedStatic]
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
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_003.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_003.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_003.php');
?>
--EXPECT--
2,4,101:5
3,5,102:6

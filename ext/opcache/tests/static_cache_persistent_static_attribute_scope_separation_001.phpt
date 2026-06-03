--TEST--
OPcache StableStatic keeps class, property, and method attribute scopes separate
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_010.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class ClassScopeState
{
	public static int $propertyValue = 0;

	public static function methodValue(): int
	{
		static $value = 0;

		return ++$value;
	}
}

class PropertyScopeState
{
	#[OPcache\StableStatic]
	public static int $propertyValue = 0;

	public static int $normalValue = 0;
}

class MethodScopeState
{
	public static int $normalValue = 0;

	#[OPcache\StableStatic]
	public static function methodValue(): int
	{
		static $value = 0;

		return ++$value;
	}

	public static function normalMethodValue(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$action = $_GET['action'] ?? 'step';

if ($action === 'reset') {
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'inspect') {
	echo ClassScopeState::$propertyValue, ',',
		ClassScopeState::methodValue(), ',',
		PropertyScopeState::$propertyValue, ',',
		MethodScopeState::methodValue(), ',',
		PropertyScopeState::$normalValue, ',',
		MethodScopeState::$normalValue, ',',
		MethodScopeState::normalMethodValue(), ',',
		OPcache\StableCache::info()->entry_count, "\n";
	return;
}

echo ++ClassScopeState::$propertyValue, ',',
	ClassScopeState::methodValue(), ',',
	++PropertyScopeState::$propertyValue, ',',
	++PropertyScopeState::$normalValue, ',',
	MethodScopeState::methodValue(), ',',
	MethodScopeState::normalMethodValue(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_010.php?action=reset');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_010.php?action=step');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_010.php?action=step');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_010.php?action=inspect');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_010.php');
?>
--EXPECT--
reset
1,1,1,1,1,1
2,2,2,1,2,1
2,3,2,3,0,0,1,3

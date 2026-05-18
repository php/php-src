--TEST--
OPcache PersistentStatic class attribute stores class-wide state under one cache key
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_static_006.php', <<<'PHP'
<?php
#[OPcache\PersistentStatic]
class CombinedBlobState
{
	public static int $propertyCount = 0;
	public static array $propertyBag = [];

	public static function nextMethod(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$request = (int) ($_GET['request'] ?? 1);
if ($request === 1) {
	CombinedBlobState::$propertyCount++;
	CombinedBlobState::$propertyBag['numbers'][] = 10;
	echo CombinedBlobState::$propertyCount, ',', CombinedBlobState::nextMethod(), "\n";
	return;
}

echo CombinedBlobState::$propertyCount, ',', CombinedBlobState::$propertyBag['numbers'][0], ',', CombinedBlobState::nextMethod(), ',', OPcache\persistent_cache_info()->entry_count, "\n";

CombinedBlobState::$propertyCount++;
CombinedBlobState::$propertyBag['numbers'][] = 11;
echo CombinedBlobState::$propertyCount, ',', array_sum(CombinedBlobState::$propertyBag['numbers']), ',', CombinedBlobState::nextMethod(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_006.php?request=1');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_006.php?request=2');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_006.php');
?>
--EXPECT--
1,1
1,10,2,1
2,21,3

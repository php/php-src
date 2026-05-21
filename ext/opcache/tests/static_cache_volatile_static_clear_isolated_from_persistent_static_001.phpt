--TEST--
OPcache VolatileStatic persists through volatile cache and can be cleared independently of PinnedStatic
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_001.php', <<<'PHP'
<?php
#[OPcache\VolatileStatic]
class CachedCounter
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

#[OPcache\PinnedStatic]
class GlobalCounter
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$action = $_GET['action'] ?? 'step';
if ($action === 'clear') {
	OPcache\volatile_clear();
	echo "clear\n";
	return;
}

echo ++CachedCounter::$value, ',', CachedCounter::next(), ',', ++GlobalCounter::$value, ',', GlobalCounter::next(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_001.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_001.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_001.php?action=clear');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_001.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_001.php');
?>
--EXPECT--
1,1,1,1
2,2,2,2
clear
1,1,3,3

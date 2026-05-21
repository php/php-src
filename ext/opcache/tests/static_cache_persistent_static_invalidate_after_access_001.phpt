--TEST--
OPcache PinnedStatic is not republished after opcache_invalidate() in the same request
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_invalidate_after_access_001.php', <<<'PHP'
<?php
#[OPcache\PinnedStatic]
class GlobalInvalidateAfterAccess
{
	public static int $value = 0;

	public static function next(): int
	{
		static $value = 0;

		return ++$value;
	}
}

$action = $_GET['action'] ?? 'step';

if ($action === 'invalidate_after_access') {
	echo ++GlobalInvalidateAfterAccess::$value, ',', GlobalInvalidateAfterAccess::next(), "\n";
	var_dump(opcache_invalidate(__FILE__, true));
	return;
}

echo ++GlobalInvalidateAfterAccess::$value, ',', GlobalInvalidateAfterAccess::next(), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_invalidate_after_access_001.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_invalidate_after_access_001.php');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_invalidate_after_access_001.php?action=invalidate_after_access');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_invalidate_after_access_001.php');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_static_invalidate_after_access_001.php');
?>
--EXPECT--
1,1
2,2
3,3
bool(true)
1,1

--TEST--
OPcache VolatileCache::info and StableCache::info report separate cache backends
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_cache_info_001.php', <<<'PHP'
<?php
#[OPcache\StableStatic]
class StableInfoCounter
{
	public static int $value = 0;
}

$action = $_GET['action'] ?? 'write';
if ($action === 'write') {
	OPcache\VolatileCache::getInstance('default')->store('explicit-volatile-key', 'volatile-value');
	echo ++StableInfoCounter::$value, "\n";
	return;
}

$volatileInfo = OPcache\VolatileCache::info();
$stableInfo = OPcache\StableCache::info();
$status = opcache_get_status();
$config = opcache_get_configuration();

echo $volatileInfo->entry_count, ',', $stableInfo->entry_count, ',', OPcache\VolatileCache::getInstance('default')->fetch('explicit-volatile-key'), ',', ++StableInfoCounter::$value, "\n";
var_dump($status['volatile_cache'] == $volatileInfo);
var_dump($status['stable_cache'] == $stableInfo);
var_dump($config['directives']['opcache.static_cache.volatile_size_mb']);
var_dump($config['directives']['opcache.static_cache.stable_size_mb']);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_cache_info_001.php?action=write');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/stable_cache_info_001.php?action=info');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_cache_info_001.php');
?>
--EXPECT--
1
1,1,volatile-value,2
bool(true)
bool(true)
int(33554432)
int(33554432)

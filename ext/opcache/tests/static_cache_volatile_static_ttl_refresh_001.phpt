--TEST--
OPcache VolatileStatic TTL is refreshed by publication
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_ttl_refresh_001.php', <<<'PHP'
<?php
class VolatileStaticTtlRefreshImmediate
{
	#[OPcache\VolatileStatic(5)]
	public static int $value = 0;
}

class VolatileStaticTtlRefreshTracking
{
	#[OPcache\VolatileStatic(5, OPcache\CacheStrategy::Tracking)]
	public static int $value = 0;
}

$action = $_GET['action'] ?? 'step';

if ($action === 'reset') {
	OPcache\VolatileCache::clear();
	opcache_reset();
	echo "reset\n";
	return;
}

$values = [
	++VolatileStaticTtlRefreshImmediate::$value,
	++VolatileStaticTtlRefreshTracking::$value,
];

echo $action, '=', implode(',', $values), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_ttl_refresh_001.php';

echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=first');
sleep(3);
echo file_get_contents($base . '?action=refreshed');
sleep(3);
echo file_get_contents($base . '?action=still-live');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_ttl_refresh_001.php');
?>
--EXPECT--
reset
first=1,1
refreshed=2,2
still-live=3,3

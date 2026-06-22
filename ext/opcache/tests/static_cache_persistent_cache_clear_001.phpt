--TEST--
OPcache StableCache::clear clears only the stable cache API namespace
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_cache_clear_001.php', <<<'PHP'
<?php

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	opcache_static_cache_volatile_reset();
	OPcache\StableCache::getInstance('default')->clear();
	var_dump(OPcache\VolatileCache::getInstance('default')->store('local', 'volatile-value'));
	OPcache\StableCache::getInstance('default')->store('shared', 'stable-value');
	return;
}

if ($action === 'clear') {
	OPcache\StableCache::getInstance('default')->clear();
	var_dump(OPcache\VolatileCache::getInstance('default')->fetch('local', 'missing-volatile'));
	var_dump(OPcache\StableCache::getInstance('default')->fetch('shared', 'missing-stable'));
	return;
}

var_dump(OPcache\VolatileCache::getInstance('default')->fetch('local', 'missing-volatile'));
var_dump(OPcache\StableCache::getInstance('default')->fetch('shared', 'missing-stable'));
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_cache_clear_001.php';
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=clear');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_cache_clear_001.php');
?>
--EXPECT--
bool(true)
string(14) "volatile-value"
string(14) "missing-stable"
string(14) "volatile-value"
string(14) "missing-stable"

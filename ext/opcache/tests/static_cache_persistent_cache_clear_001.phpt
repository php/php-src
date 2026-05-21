--TEST--
OPcache pinned_clear clears only the pinned cache API namespace
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_cache_clear_001.php', <<<'PHP'
<?php

$action = $_GET['action'] ?? 'seed';

if ($action === 'seed') {
	OPcache\volatile_clear();
	OPcache\pinned_clear();
	var_dump(OPcache\volatile_store('local', 'volatile-value'));
	OPcache\pinned_store('shared', 'pinned-value');
	return;
}

if ($action === 'clear') {
	OPcache\pinned_clear();
	var_dump(OPcache\volatile_fetch('local', 'missing-volatile'));
	var_dump(OPcache\pinned_fetch('shared', 'missing-pinned'));
	return;
}

var_dump(OPcache\volatile_fetch('local', 'missing-volatile'));
var_dump(OPcache\pinned_fetch('shared', 'missing-pinned'));
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_cache_clear_001.php';
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=clear');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/pinned_cache_clear_001.php');
?>
--EXPECT--
bool(true)
string(14) "volatile-value"
string(14) "missing-pinned"
string(14) "volatile-value"
string(14) "missing-pinned"

--TEST--
OPcache persistent_store, persistent_fetch, and persistent_exists public API
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_cache_api_001.php', <<<'PHP'
<?php

$action = $_GET['action'] ?? 'write';
if ($action === 'write') {
	OPcache\persistent_store('shared', ['v' => 1]);
	var_dump(OPcache\persistent_exists('shared'));
	return;
}

var_dump(OPcache\persistent_exists('shared'));
var_dump(OPcache\persistent_fetch('shared', 'fallback'));
var_dump(OPcache\persistent_fetch('missing'));
var_dump(OPcache\persistent_fetch('missing', 'fallback'));

OPcache\persistent_store('null', null);
var_dump(OPcache\persistent_fetch('null', 'fallback'));
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32 -d opcache.file_update_protection=0');

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_cache_api_001.php?action=write');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_cache_api_001.php?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_cache_api_001.php');
?>
--EXPECT--
bool(true)
bool(true)
array(1) {
  ["v"]=>
  int(1)
}
NULL
string(8) "fallback"
NULL

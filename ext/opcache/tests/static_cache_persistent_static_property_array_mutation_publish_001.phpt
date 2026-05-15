--TEST--
OPcache PersistentStatic publishes property array mutations immediately
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/persistent_static_004.php', <<<'PHP'
<?php
class NestedSnapshotPropertyState
{
	#[OPcache\PersistentStatic]
	public static array $propertyState = [];
}

$action = $_GET['action'] ?? 'read';

if ($action === 'reset') {
	OPcache\persistent_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	NestedSnapshotPropertyState::$propertyState = ['numbers' => [1]];
	NestedSnapshotPropertyState::$propertyState['numbers'][] = 2;
	echo 'seed=', implode(',', NestedSnapshotPropertyState::$propertyState['numbers']), "\n";
	echo 'entries=', OPcache\persistent_cache_info()['entry_count'], "\n";
	return;
}

if ($action === 'mutate') {
	NestedSnapshotPropertyState::$propertyState['numbers'][] = 3;
	echo 'mutate=', implode(',', NestedSnapshotPropertyState::$propertyState['numbers']), "\n";
	return;
}

echo 'read=', implode(',', NestedSnapshotPropertyState::$propertyState['numbers'] ?? []), "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.persistent_size_mb=32 -d opcache.optimization_level=0 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/persistent_static_004.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed');
echo file_get_contents($base . '?action=read');
echo file_get_contents($base . '?action=mutate');
echo file_get_contents($base . '?action=read');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/persistent_static_004.php');
?>
--EXPECT--
reset
seed=1,2
entries=1
read=1,2
mutate=1,2,3
read=1,2,3

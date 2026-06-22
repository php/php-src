--TEST--
OPcache StableStatic class object assignments publish a snapshot without following later object mutations
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/stable_static_object_assignment_fast_path_001.php', <<<'PHP'
<?php
class StableStaticAssignmentFastPathPayload
{
	public function __construct(public string $name)
	{
	}
}

#[OPcache\StableStatic]
class StableStaticAssignmentFastPathState
{
	public static ?StableStaticAssignmentFastPathPayload $payload = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticAssignmentTrackedState
{
	public static ?StableStaticAssignmentFastPathPayload $payload = null;
}

class StableStaticPropertyAssignmentFastPathState
{
	#[OPcache\StableStatic]
	public static ?StableStaticAssignmentFastPathPayload $payload = null;
}

$action = $_GET['action'] ?? 'read-global';
if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed-global') {
	StableStaticAssignmentFastPathState::$payload = new StableStaticAssignmentFastPathPayload('assigned');
	StableStaticAssignmentFastPathState::$payload->name = 'mutated';
	echo 'global-in-request=', StableStaticAssignmentFastPathState::$payload->name, "\n";
	return;
}

if ($action === 'read-global') {
	echo 'global-after=', StableStaticAssignmentFastPathState::$payload?->name ?? 'missing', "\n";
	return;
}

if ($action === 'seed-global-property') {
	StableStaticPropertyAssignmentFastPathState::$payload = new StableStaticAssignmentFastPathPayload('assigned');
	StableStaticPropertyAssignmentFastPathState::$payload->name = 'mutated';
	echo 'global-property-in-request=', StableStaticPropertyAssignmentFastPathState::$payload->name, "\n";
	return;
}

if ($action === 'read-global-property') {
	echo 'global-property-after=', StableStaticPropertyAssignmentFastPathState::$payload?->name ?? 'missing', "\n";
	return;
}

if ($action === 'seed-cached') {
	VolatileStaticAssignmentTrackedState::$payload = new StableStaticAssignmentFastPathPayload('assigned');
	VolatileStaticAssignmentTrackedState::$payload->name = 'mutated';
	echo 'cached-in-request=', VolatileStaticAssignmentTrackedState::$payload->name, "\n";
	return;
}

echo 'cached-after=', VolatileStaticAssignmentTrackedState::$payload?->name ?? 'missing', "\n";
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.stable_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_object_assignment_fast_path_001.php';
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed-global');
echo file_get_contents($base . '?action=read-global');
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed-global-property');
echo file_get_contents($base . '?action=read-global-property');
echo file_get_contents($base . '?action=reset');
echo file_get_contents($base . '?action=seed-cached');
echo file_get_contents($base . '?action=read-cached');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_object_assignment_fast_path_001.php');
?>
--EXPECT--
reset
global-in-request=mutated
global-after=assigned
reset
global-property-in-request=mutated
global-property-after=assigned
reset
cached-in-request=mutated
cached-after=mutated

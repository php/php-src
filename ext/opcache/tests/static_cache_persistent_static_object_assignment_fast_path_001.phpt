--TEST--
OPcache PinnedStatic class object assignments publish a snapshot without following later object mutations
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/pinned_static_object_assignment_fast_path_001.php', <<<'PHP'
<?php
class PinnedStaticAssignmentFastPathPayload
{
	public function __construct(public string $name)
	{
	}
}

#[OPcache\PinnedStatic]
class PinnedStaticAssignmentFastPathState
{
	public static ?PinnedStaticAssignmentFastPathPayload $payload = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticAssignmentTrackedState
{
	public static ?PinnedStaticAssignmentFastPathPayload $payload = null;
}

class PinnedStaticPropertyAssignmentFastPathState
{
	#[OPcache\PinnedStatic]
	public static ?PinnedStaticAssignmentFastPathPayload $payload = null;
}

$action = $_GET['action'] ?? 'read-global';
if ($action === 'reset') {
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed-global') {
	PinnedStaticAssignmentFastPathState::$payload = new PinnedStaticAssignmentFastPathPayload('assigned');
	PinnedStaticAssignmentFastPathState::$payload->name = 'mutated';
	echo 'global-in-request=', PinnedStaticAssignmentFastPathState::$payload->name, "\n";
	return;
}

if ($action === 'read-global') {
	echo 'global-after=', PinnedStaticAssignmentFastPathState::$payload?->name ?? 'missing', "\n";
	return;
}

if ($action === 'seed-global-property') {
	PinnedStaticPropertyAssignmentFastPathState::$payload = new PinnedStaticAssignmentFastPathPayload('assigned');
	PinnedStaticPropertyAssignmentFastPathState::$payload->name = 'mutated';
	echo 'global-property-in-request=', PinnedStaticPropertyAssignmentFastPathState::$payload->name, "\n";
	return;
}

if ($action === 'read-global-property') {
	echo 'global-property-after=', PinnedStaticPropertyAssignmentFastPathState::$payload?->name ?? 'missing', "\n";
	return;
}

if ($action === 'seed-cached') {
	VolatileStaticAssignmentTrackedState::$payload = new PinnedStaticAssignmentFastPathPayload('assigned');
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
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.static_cache.pinned_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/pinned_static_object_assignment_fast_path_001.php';
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
@unlink(__DIR__ . '/pinned_static_object_assignment_fast_path_001.php');
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

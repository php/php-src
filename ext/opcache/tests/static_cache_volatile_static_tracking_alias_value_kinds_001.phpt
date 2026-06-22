--TEST--
OPcache VolatileStatic tracking follows PHP alias semantics across scalar and array assignments
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/static_cache_volatile_static_tracking_alias_value_kinds_001.php', <<<'PHP'
<?php
#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTrackingScalarCopyState
{
	public static $value = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTrackingArrayCopyState
{
	public static $value = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTrackingArrayReferenceState
{
	public static $value = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTrackingScalarReferenceCallState
{
	public static $value = null;
}

class VolatileStaticTrackingObjectReferenceValue
{
	public ?string $bar = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class VolatileStaticTrackingObjectReferenceState
{
	public static ?VolatileStaticTrackingObjectReferenceValue $value = null;
}

function volatile_static_tracking_alias_mutate_string(string &$value): void
{
	$value = '2';
}

function volatile_static_tracking_alias_seed(string $case): void
{
	switch ($case) {
		case 'scalar_copy':
			$value = '1';
			VolatileStaticTrackingScalarCopyState::$value = $value;
			$value = '2';
			echo 'seed=', VolatileStaticTrackingScalarCopyState::$value, "\n";
			return;

		case 'array_copy':
			$value = ['bar' => '1'];
			VolatileStaticTrackingArrayCopyState::$value = $value;
			$value['bar'] = '2';
			echo 'seed=', VolatileStaticTrackingArrayCopyState::$value['bar'], "\n";
			return;

		case 'array_reference':
			$value = '1';
			$array = ['bar' => &$value];
			VolatileStaticTrackingArrayReferenceState::$value = $array;
			$value = '2';
			echo 'seed=', VolatileStaticTrackingArrayReferenceState::$value['bar'], "\n";
			return;

		case 'scalar_reference_call':
			$value = '1';
			VolatileStaticTrackingScalarReferenceCallState::$value = $value;
			volatile_static_tracking_alias_mutate_string($value);
			echo 'seed=', VolatileStaticTrackingScalarReferenceCallState::$value, "\n";
			return;

		case 'object_reference':
			$value = new VolatileStaticTrackingObjectReferenceValue();
			$value->bar = '1';
			VolatileStaticTrackingObjectReferenceState::$value = $value;
			$value->bar = '2';
			echo 'seed=', VolatileStaticTrackingObjectReferenceState::$value?->bar ?? 'null', "\n";
			return;

		default:
			throw new RuntimeException('unknown case');
	}
}

function volatile_static_tracking_alias_read(string $case): void
{
	switch ($case) {
		case 'scalar_copy':
			echo 'read=', VolatileStaticTrackingScalarCopyState::$value ?? 'null', "\n";
			return;

		case 'array_copy':
			echo 'read=', VolatileStaticTrackingArrayCopyState::$value['bar'] ?? 'null', "\n";
			return;

		case 'array_reference':
			echo 'read=', VolatileStaticTrackingArrayReferenceState::$value['bar'] ?? 'null', "\n";
			return;

		case 'scalar_reference_call':
			echo 'read=', VolatileStaticTrackingScalarReferenceCallState::$value ?? 'null', "\n";
			return;

		case 'object_reference':
			echo 'read=', VolatileStaticTrackingObjectReferenceState::$value?->bar ?? 'null', "\n";
			return;

		default:
			throw new RuntimeException('unknown case');
	}
}

$action = $_GET['action'] ?? 'read';
$case = $_GET['case'] ?? 'scalar_copy';

if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	volatile_static_tracking_alias_seed($case);
	return;
}

volatile_static_tracking_alias_read($case);
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/static_cache_volatile_static_tracking_alias_value_kinds_001.php';
foreach (['scalar_copy', 'array_copy', 'array_reference', 'scalar_reference_call', 'object_reference'] as $case) {
	echo file_get_contents($base . '?action=reset');
	echo $case, "\n";
	echo file_get_contents($base . '?action=seed&case=' . $case);
	echo file_get_contents($base . '?action=read&case=' . $case);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/static_cache_volatile_static_tracking_alias_value_kinds_001.php');
?>
--EXPECT--
reset
scalar_copy
seed=1
read=1
reset
array_copy
seed=1
read=1
reset
array_reference
seed=2
read=2
reset
scalar_reference_call
seed=1
read=1
reset
object_reference
seed=2
read=2

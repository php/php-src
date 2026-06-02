--TEST--
OPcache VolatileStatic tracking marks all roots sharing a mutated dependency dirty
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_tracking_shared_dependency_001.php', <<<'PHP'
<?php
class TrackingSharedLeaf
{
	public function __construct(
		public string $name,
		public array $events = [],
	) {
	}
}

class TrackingSharedPayload
{
	public function __construct(
		public TrackingSharedLeaf $leaf,
		public array $box,
		public array $refs = [],
	) {
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedClassA
{
	public static ?TrackingSharedPayload $payload = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedClassB
{
	public static ?TrackingSharedPayload $payload = null;
}

class TrackingSharedPropertyA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingSharedPayload $payload = null;
}

class TrackingSharedPropertyB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingSharedPayload $payload = null;
}

class TrackingSharedMethodA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingSharedPayload $payload = null): ?TrackingSharedPayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

class TrackingSharedMethodB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingSharedPayload $payload = null): ?TrackingSharedPayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedBlobState
{
	public static ?TrackingSharedPayload $first = null;
	public static ?TrackingSharedPayload $second = null;
}

function tracking_shared_payload(string $label): TrackingSharedPayload
{
	$leaf = new TrackingSharedLeaf($label);
	$box = ['nested' => ['count' => 0, 'leaf' => $leaf], 'events' => []];
	$payload = new TrackingSharedPayload($leaf, $box);
	$payload->refs['leaf'] = $leaf;
	$payload->refs['box'] =& $payload->box;

	return $payload;
}

function tracking_shared_pair(string $kind): array
{
	return match ($kind) {
		'class' => [TrackingSharedClassA::$payload, TrackingSharedClassB::$payload],
		'property' => [TrackingSharedPropertyA::$payload, TrackingSharedPropertyB::$payload],
		'method' => [TrackingSharedMethodA::payload(), TrackingSharedMethodB::payload()],
		'blob' => [TrackingSharedBlobState::$first, TrackingSharedBlobState::$second],
		default => throw new RuntimeException('unknown kind'),
	};
}

function tracking_shared_seed(string $kind): void
{
	$payload = tracking_shared_payload($kind);

	match ($kind) {
		'class' => [TrackingSharedClassA::$payload = $payload, TrackingSharedClassB::$payload = $payload],
		'property' => [TrackingSharedPropertyA::$payload = $payload, TrackingSharedPropertyB::$payload = $payload],
		'method' => [TrackingSharedMethodA::payload($payload), TrackingSharedMethodB::payload($payload)],
		'blob' => [TrackingSharedBlobState::$first = $payload, TrackingSharedBlobState::$second = $payload],
		default => throw new RuntimeException('unknown kind'),
	};
}

function tracking_shared_mutate(string $kind): void
{
	[$first, $second] = tracking_shared_pair($kind);
	if (!$first instanceof TrackingSharedPayload || !$second instanceof TrackingSharedPayload) {
		throw new RuntimeException('missing payload');
	}

	if ($kind !== 'blob' && $first !== $second) {
		$second = $first;
		match ($kind) {
			'class' => TrackingSharedClassB::$payload = $second,
			'property' => TrackingSharedPropertyB::$payload = $second,
			'method' => TrackingSharedMethodB::payload($second),
			default => throw new RuntimeException('unknown kind'),
		};
	}

	$first->leaf->events[] = $kind . '-leaf';
	$first->box['nested']['count']++;
	$first->box['nested']['leaf']->events[] = $kind . '-nested-leaf';
	$first->refs['box']['events'][] = $kind . '-ref-array';
}

function tracking_shared_dump(string $kind): void
{
	[$first, $second] = tracking_shared_pair($kind);
	echo $kind, '@pair=', ($first === $second ? 'same' : 'copy'), "\n";
	foreach ([$first, $second] as $index => $payload) {
		if (!$payload instanceof TrackingSharedPayload) {
			echo $kind, '#', $index, '=missing', "\n";
			continue;
		}

		echo $kind, '#', $index, '=',
			count($payload->leaf->events), ',',
			$payload->box['nested']['count'], ',',
			count($payload->box['nested']['leaf']->events), ',',
			count($payload->box['events']), ',',
			($payload->refs['leaf'] === $payload->leaf ? 'same' : 'copy'), ',',
			($payload->refs['box'] === $payload->box ? 'same' : 'copy'),
			"\n";
	}
}

$action = $_GET['action'] ?? 'read';
$kind = $_GET['kind'] ?? 'class';

if ($action === 'reset') {
	OPcache\VolatileCache::clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	tracking_shared_seed($kind);
	tracking_shared_dump($kind);
	return;
}

if ($action === 'mutate') {
	tracking_shared_mutate($kind);
	tracking_shared_dump($kind);
	return;
}

if ($action === 'read') {
	tracking_shared_dump($kind);
	return;
}

throw new RuntimeException('unknown action');
PHP);

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.volatile_size_mb=32 -d opcache.file_update_protection=0 -d opcache.jit=0');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_tracking_shared_dependency_001.php';
echo file_get_contents($base . '?action=reset');
foreach (['class', 'property', 'method', 'blob'] as $kind) {
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=mutate&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_tracking_shared_dependency_001.php');
?>
--EXPECT--
reset
class@pair=same
class#0=0,0,0,0,same,same
class#1=0,0,0,0,same,same
class@pair=same
class#0=2,1,2,1,same,same
class#1=2,1,2,1,same,same
class@pair=copy
class#0=2,1,2,1,same,same
class#1=2,1,2,1,same,same
property@pair=same
property#0=0,0,0,0,same,same
property#1=0,0,0,0,same,same
property@pair=same
property#0=2,1,2,1,same,same
property#1=2,1,2,1,same,same
property@pair=copy
property#0=2,1,2,1,same,same
property#1=2,1,2,1,same,same
method@pair=same
method#0=0,0,0,0,same,same
method#1=0,0,0,0,same,same
method@pair=same
method#0=2,1,2,1,same,same
method#1=2,1,2,1,same,same
method@pair=copy
method#0=2,1,2,1,same,same
method#1=2,1,2,1,same,same
blob@pair=same
blob#0=0,0,0,0,same,same
blob#1=0,0,0,0,same,same
blob@pair=same
blob#0=2,1,2,1,same,same
blob#1=2,1,2,1,same,same
blob@pair=same
blob#0=2,1,2,1,same,same
blob#1=2,1,2,1,same,same

--TEST--
OPcache VolatileStatic tracking publishes shared reference-cell mutations before restore and detaches roots after restore
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_tracking_shared_reference_cell_001.php', <<<'PHP'
<?php
class TrackingSharedReferenceChild
{
	public function __construct(
		public string $name,
		public array $events = [],
	) {
	}
}

class TrackingSharedReferenceNode
{
	public function __construct(
		public string $name,
		public array $events = [],
		public ?TrackingSharedReferenceChild $child = null,
	) {
	}
}

class TrackingSharedReferencePayload
{
	public $nodeRef;
	public array $refs = [];
	public object $holder;
	public array $labels = [];

	public function __construct(public string $label)
	{
		$this->holder = new stdClass();
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedReferenceClassA
{
	public static ?TrackingSharedReferencePayload $payload = null;
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedReferenceClassB
{
	public static ?TrackingSharedReferencePayload $payload = null;
}

class TrackingSharedReferencePropertyA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingSharedReferencePayload $payload = null;
}

class TrackingSharedReferencePropertyB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingSharedReferencePayload $payload = null;
}

class TrackingSharedReferenceMethodA
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingSharedReferencePayload $payload = null): ?TrackingSharedReferencePayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

class TrackingSharedReferenceMethodB
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingSharedReferencePayload $payload = null): ?TrackingSharedReferencePayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingSharedReferenceBlob
{
	public static ?TrackingSharedReferencePayload $first = null;
	public static ?TrackingSharedReferencePayload $second = null;
}

function tracking_shared_reference_make_payload(string $label, TrackingSharedReferenceNode &$shared): TrackingSharedReferencePayload
{
	$payload = new TrackingSharedReferencePayload($label);
	$payload->nodeRef =& $shared;
	$payload->refs['node'] =& $shared;
	$payload->refs['child'] =& $shared->child;
	$payload->holder->node =& $shared;
	$payload->holder->child =& $shared->child;
	$payload->labels[] = $label;

	return $payload;
}

function tracking_shared_reference_pair(string $kind): array
{
	return match ($kind) {
		'class' => [TrackingSharedReferenceClassA::$payload, TrackingSharedReferenceClassB::$payload],
		'property' => [TrackingSharedReferencePropertyA::$payload, TrackingSharedReferencePropertyB::$payload],
		'method' => [TrackingSharedReferenceMethodA::payload(), TrackingSharedReferenceMethodB::payload()],
		'blob' => [TrackingSharedReferenceBlob::$first, TrackingSharedReferenceBlob::$second],
		default => throw new RuntimeException('unknown kind'),
	};
}

function tracking_shared_reference_seed(string $kind): void
{
	$shared = new TrackingSharedReferenceNode('seed');
	$shared->child = new TrackingSharedReferenceChild('child');

	$first = tracking_shared_reference_make_payload($kind . '-A', $shared);
	$second = tracking_shared_reference_make_payload($kind . '-B', $shared);

	match ($kind) {
		'class' => [TrackingSharedReferenceClassA::$payload = $first, TrackingSharedReferenceClassB::$payload = $second],
		'property' => [TrackingSharedReferencePropertyA::$payload = $first, TrackingSharedReferencePropertyB::$payload = $second],
		'method' => [TrackingSharedReferenceMethodA::payload($first), TrackingSharedReferenceMethodB::payload($second)],
		'blob' => [TrackingSharedReferenceBlob::$first = $first, TrackingSharedReferenceBlob::$second = $second],
		default => throw new RuntimeException('unknown kind'),
	};

	$first->nodeRef->name = 'seed-updated';
	$first->nodeRef->events[] = 'seed-node';
	$first->refs['child']->name = 'child-updated';
	$first->holder->child->events[] = 'seed-child';
	$first->labels[] = 'seed-mutated';
	$second->labels[] = 'seed-observed';
}

function tracking_shared_reference_mutate(string $kind): void
{
	[$first, $second] = tracking_shared_reference_pair($kind);
	if (!$first instanceof TrackingSharedReferencePayload || !$second instanceof TrackingSharedReferencePayload) {
		throw new RuntimeException('missing payload');
	}

	$first->holder->node->name = 'mutated-again';
	$first->holder->node->events[] = 'mutate-node';
	$first->nodeRef->child->name = 'child-mutated-again';
	$first->nodeRef->child->events[] = 'mutate-child';
	$first->labels[] = 'mutated-first';
}

function tracking_shared_reference_dump_payload(string $kind, int $index, ?TrackingSharedReferencePayload $payload): void
{
	if (!$payload instanceof TrackingSharedReferencePayload) {
		echo $kind, '#', $index, '=missing', "\n";
		return;
	}

	echo $kind, '#', $index, '=',
		$payload->label, ',',
		$payload->nodeRef->name, ',',
		count($payload->nodeRef->events), ',',
		$payload->nodeRef->child->name, ',',
		count($payload->nodeRef->child->events), ',',
		count($payload->labels), ',',
		($payload->nodeRef === $payload->refs['node'] ? 'same' : 'copy'), ',',
		($payload->nodeRef === $payload->holder->node ? 'same' : 'copy'), ',',
		($payload->nodeRef->child === $payload->refs['child'] ? 'same' : 'copy'), ',',
		($payload->nodeRef->child === $payload->holder->child ? 'same' : 'copy'),
		"\n";
}

function tracking_shared_reference_dump(string $kind): void
{
	[$first, $second] = tracking_shared_reference_pair($kind);
	echo $kind, '@cross=',
		($first instanceof TrackingSharedReferencePayload && $second instanceof TrackingSharedReferencePayload && $first->nodeRef === $second->nodeRef ? 'same' : 'copy'), ',',
		($first instanceof TrackingSharedReferencePayload && $second instanceof TrackingSharedReferencePayload && $first->nodeRef->child === $second->nodeRef->child ? 'same' : 'copy'),
		"\n";
	tracking_shared_reference_dump_payload($kind, 0, $first);
	tracking_shared_reference_dump_payload($kind, 1, $second);
}

$action = $_GET['action'] ?? 'read';
$kind = $_GET['kind'] ?? 'class';

if ($action === 'reset') {
	opcache_static_cache_volatile_reset();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($action === 'seed') {
	tracking_shared_reference_seed($kind);
	tracking_shared_reference_dump($kind);
	return;
}

if ($action === 'mutate') {
	tracking_shared_reference_mutate($kind);
	tracking_shared_reference_dump($kind);
	return;
}

if ($action === 'read') {
	tracking_shared_reference_dump($kind);
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

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_tracking_shared_reference_cell_001.php';
echo file_get_contents($base . '?action=reset');
foreach (['class', 'property', 'method', 'blob'] as $kind) {
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=mutate&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_tracking_shared_reference_cell_001.php');
?>
--EXPECT--
reset
class@cross=same,same
class#0=class-A,seed-updated,1,child-updated,1,2,same,same,same,same
class#1=class-B,seed-updated,1,child-updated,1,2,same,same,same,same
class@cross=copy,copy
class#0=class-A,seed-updated,1,child-updated,1,2,same,same,same,same
class#1=class-B,seed-updated,1,child-updated,1,2,same,same,same,same
class@cross=copy,copy
class#0=class-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
class#1=class-B,seed-updated,1,child-updated,1,2,same,same,same,same
class@cross=copy,copy
class#0=class-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
class#1=class-B,seed-updated,1,child-updated,1,2,same,same,same,same
property@cross=same,same
property#0=property-A,seed-updated,1,child-updated,1,2,same,same,same,same
property#1=property-B,seed-updated,1,child-updated,1,2,same,same,same,same
property@cross=copy,copy
property#0=property-A,seed-updated,1,child-updated,1,2,same,same,same,same
property#1=property-B,seed-updated,1,child-updated,1,2,same,same,same,same
property@cross=copy,copy
property#0=property-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
property#1=property-B,seed-updated,1,child-updated,1,2,same,same,same,same
property@cross=copy,copy
property#0=property-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
property#1=property-B,seed-updated,1,child-updated,1,2,same,same,same,same
method@cross=same,same
method#0=method-A,seed-updated,1,child-updated,1,2,same,same,same,same
method#1=method-B,seed-updated,1,child-updated,1,2,same,same,same,same
method@cross=copy,copy
method#0=method-A,seed-updated,1,child-updated,1,2,same,same,same,same
method#1=method-B,seed-updated,1,child-updated,1,2,same,same,same,same
method@cross=copy,copy
method#0=method-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
method#1=method-B,seed-updated,1,child-updated,1,2,same,same,same,same
method@cross=copy,copy
method#0=method-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
method#1=method-B,seed-updated,1,child-updated,1,2,same,same,same,same
blob@cross=same,same
blob#0=blob-A,seed-updated,1,child-updated,1,2,same,same,same,same
blob#1=blob-B,seed-updated,1,child-updated,1,2,same,same,same,same
blob@cross=copy,copy
blob#0=blob-A,seed-updated,1,child-updated,1,2,same,same,same,same
blob#1=blob-B,seed-updated,1,child-updated,1,2,same,same,same,same
blob@cross=copy,copy
blob#0=blob-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
blob#1=blob-B,seed-updated,1,child-updated,1,2,same,same,same,same
blob@cross=copy,copy
blob#0=blob-A,mutated-again,2,child-mutated-again,2,3,same,same,same,same
blob#1=blob-B,seed-updated,1,child-updated,1,2,same,same,same,same

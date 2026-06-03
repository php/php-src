--TEST--
OPcache VolatileStatic tracking persists deep mutations through referenced object graphs
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_tracking_reference_object_graph_001.php', <<<'PHP'
<?php
class TrackingReferenceChild
{
	public function __construct(
		public string $name,
		public array $events = [],
	) {
	}
}

class TrackingReferenceNode
{
	public function __construct(
		public string $name,
		public array $events = [],
		public ?TrackingReferenceChild $child = null,
	) {
	}
}

class TrackingReferencePayload
{
	public $objectRef;
	public array $arrayRef = [];
	public object $holder;
	public array $log = [];
	public function __construct()
	{
		$this->holder = new stdClass();
	}
}

#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
class TrackingReferenceClassState
{
	public static ?TrackingReferencePayload $payload = null;
}

class TrackingReferencePropertyState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingReferencePayload $payload = null;
}

class TrackingReferenceMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingReferencePayload $payload = null): ?TrackingReferencePayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

function tracking_reference_set(string $kind, ?TrackingReferencePayload $payload): void
{
	switch ($kind) {
		case 'class':
			TrackingReferenceClassState::$payload = $payload;
			return;

		case 'property':
			TrackingReferencePropertyState::$payload = $payload;
			return;

		case 'method':
			TrackingReferenceMethodState::payload($payload);
			return;

		default:
			throw new RuntimeException('unknown kind');
	}
}

function tracking_reference_get(string $kind): ?TrackingReferencePayload
{
	return match ($kind) {
		'class' => TrackingReferenceClassState::$payload,
		'property' => TrackingReferencePropertyState::$payload,
		'method' => TrackingReferenceMethodState::payload(),
		default => throw new RuntimeException('unknown kind'),
	};
}

function tracking_reference_seed(string $kind): void
{
	$leaf = new TrackingReferenceNode('seed');
	$leaf->child = new TrackingReferenceChild('child');

	$payload = new TrackingReferencePayload();
	$payload->objectRef =& $leaf;
	$payload->arrayRef = [
		'alias' => &$leaf,
		'child' => &$leaf->child,
	];
	$payload->holder->alias =& $leaf;
	$payload->holder->child =& $leaf->child;
	$payload->log[] = 'seed';

	tracking_reference_set($kind, $payload);

	$leaf->name = 'seed-updated';
	$leaf->events[] = 'source-object';
	$leaf->child->name = 'child-updated';
	$leaf->child->events[] = 'source-child';
	$payload->arrayRef['alias']->events[] = 'array-alias';
	$payload->holder->alias->child->events[] = 'holder-alias-child';
	$payload->log[] = 'seed-mutated';
}

function tracking_reference_mutate(string $kind): void
{
	$payload = tracking_reference_get($kind);
	if (!$payload instanceof TrackingReferencePayload) {
		throw new RuntimeException('missing payload');
	}

	$payload->objectRef->name = 'mutated-again';
	$payload->objectRef->events[] = 'object-ref';
	$payload->arrayRef['child']->name = 'child-mutated-again';
	$payload->arrayRef['child']->events[] = 'array-child';
	$payload->holder->alias->events[] = 'holder-alias';
	$payload->holder->alias->child->events[] = 'holder-alias-child';
	$payload->log[] = 'mutated';
}

function tracking_reference_dump(string $kind, string $label): void
{
	$payload = tracking_reference_get($kind);
	if (!$payload instanceof TrackingReferencePayload) {
		echo $kind, '@', $label, '=missing', "\n";
		return;
	}

	echo $kind, '@', $label, '=',
		$payload->objectRef->name, ',',
		count($payload->objectRef->events), ',',
		$payload->objectRef->child->name, ',',
		count($payload->objectRef->child->events), ',',
		count($payload->log), ',',
		($payload->objectRef === $payload->arrayRef['alias'] ? 'same' : 'copy'), ',',
		($payload->objectRef->child === $payload->arrayRef['child'] ? 'same' : 'copy'), ',',
		($payload->holder->alias === $payload->objectRef ? 'same' : 'copy'), ',',
		($payload->holder->child === $payload->objectRef->child ? 'same' : 'copy'), ',',
		($payload->holder->alias->child === $payload->objectRef->child ? 'same' : 'copy'),
		"\n";
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
	tracking_reference_seed($kind);
	tracking_reference_dump($kind, 'seed');
	return;
}

if ($action === 'mutate') {
	tracking_reference_mutate($kind);
	tracking_reference_dump($kind, 'mutate');
	return;
}

if ($action === 'read') {
	tracking_reference_dump($kind, 'read');
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

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_tracking_reference_object_graph_001.php';
echo file_get_contents($base . '?action=reset');
foreach (['class', 'property', 'method'] as $kind) {
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=mutate&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_tracking_reference_object_graph_001.php');
?>
--EXPECT--
reset
class@seed=seed-updated,2,child-updated,2,2,same,same,same,same,same
class@read=seed-updated,2,child-updated,2,2,same,same,same,same,same
class@mutate=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same
class@read=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same
property@seed=seed-updated,2,child-updated,2,2,same,same,same,same,same
property@read=seed-updated,2,child-updated,2,2,same,same,same,same,same
property@mutate=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same
property@read=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same
method@seed=seed-updated,2,child-updated,2,2,same,same,same,same,same
method@read=seed-updated,2,child-updated,2,2,same,same,same,same,same
method@mutate=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same
method@read=mutated-again,4,child-mutated-again,4,3,same,same,same,same,same

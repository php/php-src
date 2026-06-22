--TEST--
OPcache VolatileStatic tracking reference graphs are not republished after opcache_static_cache_volatile_reset() or opcache_invalidate() in the same request
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

file_put_contents(__DIR__ . '/volatile_static_tracking_clear_invalidate_after_access_001.php', <<<'PHP'
<?php
class TrackingClearInvalidateChild
{
	public function __construct(
		public string $name,
		public array $events = [],
	) {
	}
}

class TrackingClearInvalidateNode
{
	public function __construct(
		public string $name,
		public array $events = [],
		public ?TrackingClearInvalidateChild $child = null,
	) {
	}
}

class TrackingClearInvalidatePayload
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
class TrackingClearInvalidateClassState
{
	public static ?TrackingClearInvalidatePayload $payload = null;
}

class TrackingClearInvalidatePropertyState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static ?TrackingClearInvalidatePayload $payload = null;
}

class TrackingClearInvalidateMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function payload(?TrackingClearInvalidatePayload $payload = null): ?TrackingClearInvalidatePayload
	{
		static $value = null;

		if ($payload !== null) {
			$value = $payload;
		}

		return $value;
	}
}

function tracking_clear_invalidate_set(string $kind, ?TrackingClearInvalidatePayload $payload): void
{
	switch ($kind) {
		case 'class':
			TrackingClearInvalidateClassState::$payload = $payload;
			return;

		case 'property':
			TrackingClearInvalidatePropertyState::$payload = $payload;
			return;

		case 'method':
			TrackingClearInvalidateMethodState::payload($payload);
			return;

		default:
			throw new RuntimeException('unknown kind');
	}
}

function tracking_clear_invalidate_get(string $kind): ?TrackingClearInvalidatePayload
{
	return match ($kind) {
		'class' => TrackingClearInvalidateClassState::$payload,
		'property' => TrackingClearInvalidatePropertyState::$payload,
		'method' => TrackingClearInvalidateMethodState::payload(),
		default => throw new RuntimeException('unknown kind'),
	};
}

function tracking_clear_invalidate_apply(TrackingClearInvalidatePayload $payload, string $nodeName, string $childName, string $label): void
{
	$payload->objectRef->name = $nodeName;
	$payload->objectRef->events[] = $label . '-object';
	$payload->arrayRef['child']->name = $childName;
	$payload->arrayRef['child']->events[] = $label . '-child';
	$payload->holder->alias->events[] = $label . '-holder';
	$payload->log[] = $label;
}

function tracking_clear_invalidate_seed(string $kind): void
{
	$leaf = new TrackingClearInvalidateNode('seed');
	$leaf->child = new TrackingClearInvalidateChild('child');

	$payload = new TrackingClearInvalidatePayload();
	$payload->objectRef =& $leaf;
	$payload->arrayRef = [
		'alias' => &$leaf,
		'child' => &$leaf->child,
	];
	$payload->holder->alias =& $leaf;
	$payload->holder->child =& $leaf->child;
	$payload->log[] = 'seed';

	tracking_clear_invalidate_set($kind, $payload);
	tracking_clear_invalidate_apply($payload, 'seed-node', 'seed-child', 'seed');
}

function tracking_clear_invalidate_dump(string $kind, string $label): void
{
	$payload = tracking_clear_invalidate_get($kind);
	if (!$payload instanceof TrackingClearInvalidatePayload) {
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
	tracking_clear_invalidate_seed($kind);
	tracking_clear_invalidate_dump($kind, 'seed');
	return;
}

if ($action === 'clear_after_access') {
	$payload = tracking_clear_invalidate_get($kind);
	if (!$payload instanceof TrackingClearInvalidatePayload) {
		throw new RuntimeException('missing payload');
	}

	tracking_clear_invalidate_apply($payload, 'clear-node', 'clear-child', 'clear');
	tracking_clear_invalidate_dump($kind, 'clear-before');
	opcache_static_cache_volatile_reset();
	echo "clear\n";
	return;
}

if ($action === 'reset_after_access') {
	$payload = tracking_clear_invalidate_get($kind);
	if (!$payload instanceof TrackingClearInvalidatePayload) {
		throw new RuntimeException('missing payload');
	}

	tracking_clear_invalidate_apply($payload, 'reset-node', 'reset-child', 'reset');
	tracking_clear_invalidate_dump($kind, 'reset-before');
	opcache_reset();
	echo "reset-after\n";
	return;
}

if ($action === 'invalidate_after_access') {
	$payload = tracking_clear_invalidate_get($kind);
	if (!$payload instanceof TrackingClearInvalidatePayload) {
		throw new RuntimeException('missing payload');
	}

	tracking_clear_invalidate_apply($payload, 'invalidate-node', 'invalidate-child', 'invalidate');
	tracking_clear_invalidate_dump($kind, 'invalidate-before');
	var_dump(opcache_invalidate(__FILE__, true));
	return;
}

if ($action === 'read') {
	tracking_clear_invalidate_dump($kind, 'read');
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

$base = 'http://' . PHP_CLI_SERVER_ADDRESS . '/volatile_static_tracking_clear_invalidate_after_access_001.php';
echo file_get_contents($base . '?action=reset');
foreach (['class', 'property', 'method'] as $kind) {
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=clear_after_access&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=reset_after_access&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=seed&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
	echo file_get_contents($base . '?action=invalidate_after_access&kind=' . $kind);
	echo file_get_contents($base . '?action=read&kind=' . $kind);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/volatile_static_tracking_clear_invalidate_after_access_001.php');
?>
--EXPECT--
reset
class@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
class@read=seed-node,2,seed-child,1,2,same,same,same,same,same
class@clear-before=clear-node,4,clear-child,2,3,same,same,same,same,same
clear
class@read=missing
class@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
class@read=seed-node,2,seed-child,1,2,same,same,same,same,same
class@reset-before=reset-node,4,reset-child,2,3,same,same,same,same,same
reset-after
class@read=missing
class@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
class@read=seed-node,2,seed-child,1,2,same,same,same,same,same
class@invalidate-before=invalidate-node,4,invalidate-child,2,3,same,same,same,same,same
bool(true)
class@read=missing
property@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
property@read=seed-node,2,seed-child,1,2,same,same,same,same,same
property@clear-before=clear-node,4,clear-child,2,3,same,same,same,same,same
clear
property@read=missing
property@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
property@read=seed-node,2,seed-child,1,2,same,same,same,same,same
property@reset-before=reset-node,4,reset-child,2,3,same,same,same,same,same
reset-after
property@read=missing
property@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
property@read=seed-node,2,seed-child,1,2,same,same,same,same,same
property@invalidate-before=invalidate-node,4,invalidate-child,2,3,same,same,same,same,same
bool(true)
property@read=missing
method@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
method@read=seed-node,2,seed-child,1,2,same,same,same,same,same
method@clear-before=clear-node,4,clear-child,2,3,same,same,same,same,same
clear
method@read=missing
method@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
method@read=seed-node,2,seed-child,1,2,same,same,same,same,same
method@reset-before=reset-node,4,reset-child,2,3,same,same,same,same,same
reset-after
method@read=missing
method@seed=seed-node,2,seed-child,1,2,same,same,same,same,same
method@read=seed-node,2,seed-child,1,2,same,same,same,same,same
method@invalidate-before=invalidate-node,4,invalidate-child,2,3,same,same,same,same,same
bool(true)
method@read=missing
--TEST--
OPcache VolatileStatic tracking persists deep mutations through referenced object graphs under FPM
--EXTENSIONS--
opcache
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[unconfined]
listen = {{ADDR}}
pm = static
pm.max_children = 1
EOT;

$code = <<<'EOT'
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
	OPcache\volatile_clear();
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

tracking_reference_dump($kind, 'read');
EOT;

$args = [];
if (file_exists(ini_get('extension_dir') . '/opcache.so')) {
	$args[] = '-dzend_extension=' . ini_get('extension_dir') . '/opcache.so';
}
$args = [
	...$args,
	'-dopcache.enable=1',
	'-dopcache.static_cache.volatile_size_mb=32',
	'-dopcache.file_update_protection=0',
	'-dopcache.jit=0',
];

$tester = new FPM\Tester($cfg, $code);
$tester->start($args);
$tester->expectLogStartNotices();

function tracking_reference_fpm_request(FPM\Tester $tester, string $query): void
{
	echo $tester->request($query)->getBody(), "\n";
}

tracking_reference_fpm_request($tester, 'action=reset');
foreach (['class', 'property', 'method'] as $kind) {
	tracking_reference_fpm_request($tester, 'action=seed&kind=' . $kind);
	tracking_reference_fpm_request($tester, 'action=read&kind=' . $kind);
	tracking_reference_fpm_request($tester, 'action=mutate&kind=' . $kind);
	tracking_reference_fpm_request($tester, 'action=read&kind=' . $kind);
}

$tester->terminate();
$tester->close();
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
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

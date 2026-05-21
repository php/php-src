--TEST--
FPM: OPcache VolatileStatic tracks internal object method mutations
--EXTENSIONS--
opcache
spl
--SKIPIF--
<?php include __DIR__ . '/skipif.inc'; ?>
--FILE--
<?php

require_once __DIR__ . '/tester.inc';

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}
[opcache]
listen = {{ADDR:UDS}}
pm = static
pm.max_children = 1
pm.max_requests = 0
catch_workers_output = yes
EOT;

$code = <<<'PHP'
<?php

class VolatileStaticDateMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): DateTime
	{
		static $value = null;

		$value ??= new DateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'));
		return $value;
	}
}

class VolatileStaticArrayObjectMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): ArrayObject
	{
		static $value = null;

		$value ??= new ArrayObject(['count' => 0]);
		return $value;
	}
}

class VolatileStaticFixedArrayMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): SplFixedArray
	{
		static $value = null;

		$value ??= SplFixedArray::fromArray(['seed']);
		return $value;
	}
}

$action = $_GET['action'] ?? 'read';
$state = $_GET['state'] ?? 'date';

if ($action === 'reset') {
	OPcache\volatile_clear();
	opcache_reset();
	echo "reset\n";
	return;
}

if ($state === 'date') {
	$value = VolatileStaticDateMethodState::value();
	if ($action === 'mutate') {
		$value->modify('+1 day');
	}
	echo "date=", $value->format('Y-m-d'), "\n";
	return;
}

if ($state === 'array') {
	$value = VolatileStaticArrayObjectMethodState::value();
	if ($action === 'mutate') {
		$value->offsetSet('count', $value->offsetGet('count') + 1);
		$value->append('tail');
	}
	echo "array=", $value->offsetGet('count'), ",", count($value), "\n";
	return;
}

$value = VolatileStaticFixedArrayMethodState::value();
if ($action === 'mutate') {
	$value->offsetSet(0, 'changed');
	$value->setSize(2);
	$value->offsetSet(1, 'tail');
}
$second = $value->count() > 1 ? $value->offsetGet(1) : 'none';
echo "fixed=", $value->count(), ",", $value->offsetGet(0), ",", $second, "\n";
PHP;

$filePrefix = sys_get_temp_dir() . '/opcache_static_cache_internal_object_fpm_' . getmypid() . '.';
$tester = new FPM\Tester($cfg, $code, fileName: $filePrefix);
$tester->start(iniEntries: [
	'opcache.enable' => '1',
	'opcache.static_cache.volatile_size_mb' => '32',
	'opcache.file_update_protection' => '0',
	'opcache.jit' => '0',
]);
register_shutdown_function(static function () use ($filePrefix): void {
	foreach (glob($filePrefix . '*') ?: [] as $path) {
		@unlink($path);
	}
});
$tester->expectLogStartNotices();

function static_cache_internal_object_fpm_request(FPM\Tester $tester, string $query): void
{
	echo $tester->request($query, address: '{{ADDR:UDS}}')->getBody(), "\n";
}

foreach (['date', 'array', 'fixed'] as $state) {
	static_cache_internal_object_fpm_request($tester, 'action=reset');
	static_cache_internal_object_fpm_request($tester, 'state=' . $state . '&action=read');
	static_cache_internal_object_fpm_request($tester, 'state=' . $state . '&action=mutate');
	static_cache_internal_object_fpm_request($tester, 'state=' . $state . '&action=read');
}

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
?>
--EXPECT--
reset
date=2026-01-01
date=2026-01-02
date=2026-01-02
reset
array=0,1
array=1,2
array=1,2
reset
fixed=1,seed,none
fixed=2,changed,tail
fixed=2,changed,tail
--CLEAN--
<?php
require_once __DIR__ . '/tester.inc';
FPM\Tester::clean();
?>

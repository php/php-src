--TEST--
OPcache explicit fetchMultiple uses request-local slots with per-entry generations
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

final class ExplicitFetchMultipleGenerationPayload
{
	public static int $unserializeCount = 0;

	public function __construct(public string $label) {}

	public function __serialize(): array
	{
		return ['label' => $this->label];
	}

	public function __unserialize(array $data): void
	{
		self::$unserializeCount++;
		$this->label = $data['label'];
	}
}

function check_fetch_multiple_generation(string $label, OPcache\StaticCacheInterface $cache): void
{
	ExplicitFetchMultipleGenerationPayload::$unserializeCount = 0;

	$cache->storeMultiple([
		'a' => new ExplicitFetchMultipleGenerationPayload($label . '-a'),
		'b' => new ExplicitFetchMultipleGenerationPayload($label . '-b'),
	]);

	$first = $cache->fetchMultiple(['a', 'b', 'missing'], ['fallback']);
	echo $label, '-keys=', implode(',', array_keys($first)), "\n";
	echo $label, '-first=', $first['a']->label, ':', $first['b']->label, ':',
		$first['missing'][0], ':', ExplicitFetchMultipleGenerationPayload::$unserializeCount, "\n";

	$first['a']->label = $label . '-mutated';
	$second = $cache->fetchMultiple(['a', 'b'], ['fallback']);
	echo $label, '-second=', $second['a']->label, ':', $second['b']->label, ':',
		ExplicitFetchMultipleGenerationPayload::$unserializeCount, "\n";

	$cache->store('other', new ExplicitFetchMultipleGenerationPayload($label . '-other'));
	$third = $cache->fetchMultiple(['a', 'b'], ['fallback']);
	echo $label, '-third-after-other=', $third['a']->label, ':', $third['b']->label, ':',
		ExplicitFetchMultipleGenerationPayload::$unserializeCount, "\n";

	$cache->store('a', new ExplicitFetchMultipleGenerationPayload($label . '-new-a'));
	$fourth = $cache->fetchMultiple(['a', 'b'], ['fallback']);
	echo $label, '-fourth-after-same=', $fourth['a']->label, ':', $fourth['b']->label, ':',
		ExplicitFetchMultipleGenerationPayload::$unserializeCount, "\n";

	$cache->delete('a');
	$fifth = $cache->fetchMultiple(['a', 'b'], ['fallback']);
	echo $label, '-fifth-after-delete=', $fifth['a'][0], ':', $fifth['b']->label, ':',
		ExplicitFetchMultipleGenerationPayload::$unserializeCount, "\n";
}

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('fetch_multiple_generation')->clear();

check_fetch_multiple_generation(
	'volatile',
	OPcache\VolatileCache::getInstance('fetch_multiple_generation')
);
check_fetch_multiple_generation(
	'stable',
	OPcache\StableCache::getInstance('fetch_multiple_generation')
);

?>
--EXPECT--
volatile-keys=a,b,missing
volatile-first=volatile-a:volatile-b:fallback:2
volatile-second=volatile-a:volatile-b:2
volatile-third-after-other=volatile-a:volatile-b:2
volatile-fourth-after-same=volatile-new-a:volatile-b:3
volatile-fifth-after-delete=fallback:volatile-b:3
stable-keys=a,b,missing
stable-first=stable-a:stable-b:fallback:2
stable-second=stable-a:stable-b:2
stable-third-after-other=stable-a:stable-b:2
stable-fourth-after-same=stable-new-a:stable-b:3
stable-fifth-after-delete=fallback:stable-b:3

--TEST--
OPcache explicit cache preserves earlier snapshots across repeated stores from one source value
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

final class PreparedScratchPayload
{
	public function __construct(
		public string $name,
		public array $rows,
	) {}
}

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 256; $i++) {
		$rows[] = [
			'id' => $i,
			'label' => str_repeat($prefix, 24),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
}

opcache_static_cache_volatile_reset();
OPcache\StableCache::getInstance('default')->clear();

$payload = new PreparedScratchPayload('first', build_rows('A', 3));

var_dump(OPcache\VolatileCache::getInstance('default')->store('scratch_volatile_first', $payload));
OPcache\StableCache::getInstance('default')->store('scratch_stable_first', $payload);

$payload->name = 'second';
$payload->rows[12]['label'] = str_repeat('B', 24);
$payload->rows[12]['nested']['value'] = 777;

var_dump(OPcache\VolatileCache::getInstance('default')->store('scratch_volatile_second', $payload));
OPcache\StableCache::getInstance('default')->store('scratch_stable_second', $payload);

$volatileFirst = OPcache\VolatileCache::getInstance('default')->fetch('scratch_volatile_first');
$volatileSecond = OPcache\VolatileCache::getInstance('default')->fetch('scratch_volatile_second');
$stableFirst = OPcache\StableCache::getInstance('default')->fetch('scratch_stable_first');
$stableSecond = OPcache\StableCache::getInstance('default')->fetch('scratch_stable_second');

echo $volatileFirst->name, "\n";
echo $volatileFirst->rows[12]['label'], "\n";
echo $volatileFirst->rows[12]['nested']['value'], "\n";
echo $volatileSecond->name, "\n";
echo $volatileSecond->rows[12]['label'], "\n";
echo $volatileSecond->rows[12]['nested']['value'], "\n";
echo $stableFirst->name, "\n";
echo $stableFirst->rows[12]['label'], "\n";
echo $stableFirst->rows[12]['nested']['value'], "\n";
echo $stableSecond->name, "\n";
echo $stableSecond->rows[12]['label'], "\n";
echo $stableSecond->rows[12]['nested']['value'], "\n";

?>
--EXPECT--
bool(true)
bool(true)
first
AAAAAAAAAAAAAAAAAAAAAAAA
36
second
BBBBBBBBBBBBBBBBBBBBBBBB
777
first
AAAAAAAAAAAAAAAAAAAAAAAA
36
second
BBBBBBBBBBBBBBBBBBBBBBBB
777

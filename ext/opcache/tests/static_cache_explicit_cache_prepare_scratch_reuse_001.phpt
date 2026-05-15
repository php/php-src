--TEST--
OPcache explicit cache preserves earlier snapshots across repeated stores from one source value
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.persistent_size_mb=32
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

OPcache\volatile_clear();
OPcache\persistent_clear();

$payload = new PreparedScratchPayload('first', build_rows('A', 3));

var_dump(OPcache\volatile_store('scratch_volatile_first', $payload));
OPcache\persistent_store('scratch_persistent_first', $payload);

$payload->name = 'second';
$payload->rows[12]['label'] = str_repeat('B', 24);
$payload->rows[12]['nested']['value'] = 777;

var_dump(OPcache\volatile_store('scratch_volatile_second', $payload));
OPcache\persistent_store('scratch_persistent_second', $payload);

$volatileFirst = OPcache\volatile_fetch('scratch_volatile_first');
$volatileSecond = OPcache\volatile_fetch('scratch_volatile_second');
$persistentFirst = OPcache\persistent_fetch('scratch_persistent_first');
$persistentSecond = OPcache\persistent_fetch('scratch_persistent_second');

echo $volatileFirst->name, "\n";
echo $volatileFirst->rows[12]['label'], "\n";
echo $volatileFirst->rows[12]['nested']['value'], "\n";
echo $volatileSecond->name, "\n";
echo $volatileSecond->rows[12]['label'], "\n";
echo $volatileSecond->rows[12]['nested']['value'], "\n";
echo $persistentFirst->name, "\n";
echo $persistentFirst->rows[12]['label'], "\n";
echo $persistentFirst->rows[12]['nested']['value'], "\n";
echo $persistentSecond->name, "\n";
echo $persistentSecond->rows[12]['label'], "\n";
echo $persistentSecond->rows[12]['nested']['value'], "\n";

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

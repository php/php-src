--TEST--
OPcache explicit cache preserves earlier snapshots across repeated stores from one source value
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
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
OPcache\pinned_clear();

$payload = new PreparedScratchPayload('first', build_rows('A', 3));

var_dump(OPcache\volatile_store('scratch_volatile_first', $payload));
OPcache\pinned_store('scratch_pinned_first', $payload);

$payload->name = 'second';
$payload->rows[12]['label'] = str_repeat('B', 24);
$payload->rows[12]['nested']['value'] = 777;

var_dump(OPcache\volatile_store('scratch_volatile_second', $payload));
OPcache\pinned_store('scratch_pinned_second', $payload);

$volatileFirst = OPcache\volatile_fetch('scratch_volatile_first');
$volatileSecond = OPcache\volatile_fetch('scratch_volatile_second');
$pinnedFirst = OPcache\pinned_fetch('scratch_pinned_first');
$pinnedSecond = OPcache\pinned_fetch('scratch_pinned_second');

echo $volatileFirst->name, "\n";
echo $volatileFirst->rows[12]['label'], "\n";
echo $volatileFirst->rows[12]['nested']['value'], "\n";
echo $volatileSecond->name, "\n";
echo $volatileSecond->rows[12]['label'], "\n";
echo $volatileSecond->rows[12]['nested']['value'], "\n";
echo $pinnedFirst->name, "\n";
echo $pinnedFirst->rows[12]['label'], "\n";
echo $pinnedFirst->rows[12]['nested']['value'], "\n";
echo $pinnedSecond->name, "\n";
echo $pinnedSecond->rows[12]['label'], "\n";
echo $pinnedSecond->rows[12]['nested']['value'], "\n";

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

--TEST--
OPcache volatile cache fetched shared graph survives same-request overwrite and reuse
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 8000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($prefix, 64),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
}

function build_payload(string $prefix, int $multiplier): array
{
	return [
		'name' => 'payload-' . $prefix,
		'rows' => build_rows($prefix, $multiplier),
	];
}

$key = 'materialized_overwrite_payload';
$reuseKey = 'materialized_overwrite_reuse_payload';

OPcache\volatile_clear();
if (!OPcache\volatile_store($key, build_payload('A', 3))) {
	throw new RuntimeException('initial store failed');
}

$fetched = OPcache\volatile_fetch($key);
$before = $fetched['rows'][123]['text'];

if (!OPcache\volatile_store($key, build_payload('B', 7))) {
	throw new RuntimeException('overwrite store failed');
}
if (!OPcache\volatile_store($reuseKey, build_payload('C', 11))) {
	throw new RuntimeException('reuse store failed');
}

$after = $fetched['rows'][123]['text'];
$nested = $fetched['rows'][123]['nested']['value'];
$refetched = OPcache\volatile_fetch($key);
$reused = OPcache\volatile_fetch($reuseKey);

echo $before, "\n";
echo $after, "\n";
echo $nested, "\n";
echo $refetched['rows'][123]['text'], "\n";
echo $refetched['rows'][123]['nested']['value'], "\n";
echo $reused['rows'][123]['text'], "\n";
echo $reused['rows'][123]['nested']['value'], "\n";

?>
--EXPECT--
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
369
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
861
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
1353
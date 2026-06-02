--TEST--
OPcache volatile cache combined entries survive delete and clear without corrupting fetched values
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

class CombinedDeleteClearNode
{
	public function __construct(public array $rows) {}
}

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 3000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($prefix, 64),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
}

function build_graph(string $prefix, int $multiplier): CombinedDeleteClearNode
{
	return new CombinedDeleteClearNode(build_rows($prefix, $multiplier));
}

function build_refs(): array
{
	$payload = ['value' => str_repeat('R', 256)];
	$payload['alias'] =& $payload['value'];

	return $payload;
}

OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::set('keep-string', str_repeat('S', 1200000)));
var_dump(OPcache\VolatileCache::set('victim-graph', build_graph('O', 3)));
var_dump(OPcache\VolatileCache::set('keep-refs', build_refs()));

$fetched = OPcache\VolatileCache::get('victim-graph');
OPcache\VolatileCache::delete('victim-graph');

var_dump(OPcache\VolatileCache::get('victim-graph', 'missing'));
var_dump($fetched instanceof CombinedDeleteClearNode);
var_dump($fetched->rows[123]['text']);

$fetched->rows[123]['text'] = 'local-after-delete';
var_dump($fetched->rows[123]['text']);

var_dump(OPcache\VolatileCache::set('replacement-graph', build_graph('N', 7)));

$replacement = OPcache\VolatileCache::get('replacement-graph');
$refs = OPcache\VolatileCache::get('keep-refs');
$refs['alias'] = 'mutated-via-alias';

var_dump($replacement->rows[123]['text']);
var_dump($replacement->rows[123]['nested']['value']);
var_dump($refs['value']);

$cleared = OPcache\VolatileCache::get('replacement-graph');
OPcache\VolatileCache::clear();

var_dump(OPcache\VolatileCache::get('keep-string', 'missing'));
var_dump(OPcache\VolatileCache::get('replacement-graph', 'missing'));
var_dump($cleared->rows[321]['text']);

$cleared->rows[321]['text'] = 'local-after-clear';
var_dump($cleared->rows[321]['text']);

var_dump(OPcache\VolatileCache::set('after-clear-string', str_repeat('A', 1200000)));
var_dump(OPcache\VolatileCache::set('after-clear-graph', build_graph('C', 11)));

$afterClear = OPcache\VolatileCache::get('after-clear-graph');

var_dump(strlen(OPcache\VolatileCache::get('after-clear-string')));
var_dump($afterClear->rows[321]['text']);
var_dump($afterClear->rows[321]['nested']['value']);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(7) "missing"
bool(true)
string(64) "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
string(18) "local-after-delete"
bool(true)
string(64) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
int(861)
string(17) "mutated-via-alias"
string(7) "missing"
string(7) "missing"
string(64) "NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"
string(17) "local-after-clear"
bool(true)
bool(true)
int(1200000)
string(64) "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
int(3531)

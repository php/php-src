--TEST--
OPcache volatile cache shared graph supports packed arrays with object elements
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

class PackedGraphItem
{
	public function __construct(
		public string $name,
		public array $metadata,
	) {}
}

class PackedGraphPayload
{
	public function __construct(public array $items) {}
}

function build_packed_graph_items(): array
{
	$items = [];
	for ($index = 0; $index < 16; $index++) {
		$items[] = new PackedGraphItem('item-' . $index, ['index' => $index, 'bucket' => $index % 4]);
	}

	return $items;
}

$items = build_packed_graph_items();
$payload = new PackedGraphPayload($items);

var_dump(OPcache\VolatileCache::set('packed_graph_payload', $payload));
var_dump(OPcache\VolatileCache::set('packed_graph_root', $items));

$payloadCopy = OPcache\VolatileCache::get('packed_graph_payload');
$rootCopy = OPcache\VolatileCache::get('packed_graph_root');

var_dump($payloadCopy instanceof PackedGraphPayload);
var_dump($payloadCopy->items[7] instanceof PackedGraphItem);
var_dump($payloadCopy->items[7]->name);
var_dump($payloadCopy->items[7]->metadata['bucket']);
var_dump($rootCopy[12] instanceof PackedGraphItem);
var_dump($rootCopy[12]->name);
var_dump($rootCopy[12]->metadata['index']);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(6) "item-7"
int(3)
bool(true)
string(7) "item-12"
int(12)

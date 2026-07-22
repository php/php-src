--TEST--
UserCache\Cache: packed dynamic arrays preserve append semantics
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class PackedDynamicArrayNode {
	public function __construct(public int $id) {
	}
}

$cache = UserCache\Cache::getPool('packed-dynamic-array');
$payload = [
	new PackedDynamicArrayNode(1),
	new PackedDynamicArrayNode(2),
	[
		'nested' => [
			new PackedDynamicArrayNode(3),
			new PackedDynamicArrayNode(4),
		],
	],
];

var_dump($cache->store('payload', $payload));

$fetched = $cache->fetch('payload');
var_dump(array_is_list($fetched));
echo implode(',', array_keys($fetched)), "\n";

$fetched[] = new PackedDynamicArrayNode(5);
echo implode(',', array_keys($fetched)), "\n";
echo $fetched[3]->id, "\n";

var_dump(array_is_list($fetched[2]['nested']));
$fetched[2]['nested'][] = new PackedDynamicArrayNode(6);
echo implode(',', array_keys($fetched[2]['nested'])), "\n";
echo $fetched[2]['nested'][2]->id, "\n";

$again = $cache->fetch('payload');
echo count($again), "\n";
echo count($again[2]['nested']), "\n";
?>
--EXPECT--
bool(true)
bool(true)
0,1,2
0,1,2,3
5
bool(true)
0,1,2
6
3
2

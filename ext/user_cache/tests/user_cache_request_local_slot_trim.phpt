--TEST--
UserCache\Cache: when the request-local slots are full, the oldest half is dropped so newly read graphs still get a prototype
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip measures memory_get_usage()');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=128M
memory_limit=64M
--FILE--
<?php
use UserCache\Cache;

class Node
{
    public $id;
    public $children = [];
}

function graph(int $id, int $children): Node
{
    $node = new Node;
    $node->id = $id;
    for ($i = 0; $i < $children; $i++) {
        $child = new Node;
        $child->id = $i;
        $node->children[] = $child;
    }

    return $node;
}

/* The second fetch of a key builds its prototype, which stays in request memory. */
function prototype_retained(Cache $cache, string $key): bool
{
    $cache->fetch($key);
    $before = memory_get_usage();
    $cache->fetch($key);

    return memory_get_usage() > $before;
}

$cache = Cache::getPool('request-local-slot-trim');
$cache->clear();

echo "slot count:\n";
for ($i = 0; $i < 4200; $i++) {
    $cache->store("marked$i", graph($i, 1));
}
$cache->store('late', graph(-1, 200));
for ($i = 0; $i < 4200; $i++) {
    $cache->fetch("marked$i");
}
var_dump(prototype_retained($cache, 'late'));

echo "slot budget:\n";
for ($i = 0; $i < 40; $i++) {
    $cache->store("big$i", graph($i, 3000));
    $cache->fetch("big$i");
    $cache->fetch("big$i");
}
$cache->store('after-budget', graph(-2, 3000));
var_dump(prototype_retained($cache, 'after-budget'));

echo "value larger than the budget:\n";
$huge = new Node;
$huge->id = str_repeat('h', 9 << 20);
$cache->store('huge', $huge);
unset($huge);
$before = memory_get_usage();
for ($i = 0; $i < 3; $i++) {
    $cache->fetch('huge');
}
var_dump(memory_get_usage() >= $before);
?>
--EXPECT--
slot count:
bool(true)
slot budget:
bool(true)
value larger than the budget:
bool(true)

--TEST--
UserCache\Cache: legacy serialize handlers get the native unserialize context, unrestorable internal classes with __sleep() are refused when storing, and references are restored before typed properties see them
--EXTENSIONS--
zend_test
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
class Node
{
    public ?Node $self = null;
    public array $list = [];
}

$cache = UserCache\Cache::getPool('internal-serialization-handlers');

$shared = new stdClass();
$shared->n = 1;
$legacy = new ZendTestLegacySerializeObject();
$legacy->data = [$shared, $shared, 'tail'];
var_dump($cache->store('legacy', $legacy));
$fetched = $cache->fetch('legacy');
var_dump(serialize($fetched) === serialize(unserialize(serialize($legacy))));
var_dump($fetched->data[0] === $fetched->data[1]);

foreach ([
    'direct' => new ZendTestSleepObjectWithCustomCreate(),
    'nested' => new ZendTestSleepObject([new ZendTestSleepObjectWithCustomCreate()]),
] as $key => $unrestorable) {
    try {
        var_dump($cache->store($key, $unrestorable));
    } catch (TypeError $e) {
        echo $key, ': ', $e->getMessage(), "\n";
    }
    var_dump($cache->has($key));
}

$node = new Node();
$reference = $node;
$node->self = &$reference;
$node->list = [&$reference];
$value = new ZendTestSleepObject([&$reference, $node]);
var_dump($cache->store('typed-reference', $value));
$fetched = $cache->fetch('typed-reference');
var_dump(serialize($fetched) === serialize(unserialize(serialize($value))));
var_dump($fetched->public[1]->self === $fetched->public[1]);

/* Release builds do not collect cycles at shutdown. */
$node = $reference = $value = $fetched = null;
gc_collect_cycles();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
direct: ZendTestSleepObjectWithCustomCreate objects cannot be stored in the user cache
bool(false)
nested: ZendTestSleepObjectWithCustomCreate objects cannot be stored in the user cache
bool(false)
bool(true)
bool(true)
bool(true)

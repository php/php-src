--TEST--
UserCache\Cache: a locked-path fetch decodes outside the cache lock, so destructors run by the cycle collector can write to the cache
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_FORCE_LOCKED_FETCH=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
class StoringGarbage
{
    public $self;

    public function __destruct()
    {
        global $cache, $stores;

        if ($cache->store('from-destructor', ++$stores)) {
            return;
        }

        echo "store from destructor failed\n";
    }
}

$cache = UserCache\Cache::getPool('locked-fetch-gc-destructors');
$stores = 0;

$scalar = 1;
$inner = ['scalar' => &$scalar, 'text' => 'v'];
$value = ['a' => &$inner, 'b' => &$inner];
var_dump($cache->store('value', $value));
unset($value, $inner, $scalar);

for ($round = 0; $round < 3; $round++) {
    gc_collect_cycles();
    $status = gc_status();
    for ($i = $status['roots']; $i < $status['threshold'] - 1; $i++) {
        $garbage = new StoringGarbage();
        $garbage->self = $garbage;
        unset($garbage);
    }

    $fetched = $cache->fetch('value');
    $fetched['a']['scalar'] = 2;
    var_dump($fetched['b']['scalar'], $fetched['a']['text']);
    unset($fetched);
}

gc_collect_cycles();
var_dump($stores > 0, $cache->fetch('from-destructor') === $stores);
?>
--EXPECT--
bool(true)
int(2)
string(1) "v"
int(2)
string(1) "v"
int(2)
string(1) "v"
bool(true)
bool(true)

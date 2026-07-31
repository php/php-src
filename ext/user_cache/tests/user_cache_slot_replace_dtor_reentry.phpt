--TEST--
UserCache\Cache: replacing a request-local slot survives a destructor that clears the pool
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class SlotReplaceDtor
{
    public int $n = 1;
    public static bool $armed = false;

    public function __destruct()
    {
        if (self::$armed) {
            UserCache\Cache::getPool('slot-replace')->clear();
        }
    }
}

$cache = UserCache\Cache::getPool('slot-replace');
$cache->clear();

$obj = new SlotReplaceDtor();
var_dump($cache->store('k', $obj));

$first = $cache->fetch('k');
$second = $cache->fetch('k');
var_dump($second->n);

SlotReplaceDtor::$armed = true;
var_dump($cache->store('k', str_repeat('a', 300)));
SlotReplaceDtor::$armed = false;

var_dump($cache->fetch('k'));
echo "done\n";
?>
--EXPECT--
bool(true)
int(1)
bool(true)
NULL
done

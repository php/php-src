--TEST--
UserCache\Cache: clear() survives request-local slot destructors that re-enter clear()/deletePool()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class ClearReentrantDtor
{
    public static bool $armed = false;

    public int $n = 1;

    public function __destruct()
    {
        if (self::$armed) {
            UserCache\Cache::getPool('clear-dtor-reentry')->clear();
        }
    }
}

class DeletePoolReentrantDtor
{
    public static bool $armed = false;

    public int $n = 2;

    public function __destruct()
    {
        if (self::$armed) {
            self::$armed = false;

            UserCache\Cache::deletePool('clear-dtor-reentry');
        }
    }
}

function seed_request_local_slot(UserCache\Cache $cache, string $key, object $value): void
{
    $cache->store($key, $value);

    $marked = $cache->fetch($key);
    unset($marked);

    $seeded = $cache->fetch($key);
    unset($seeded);
}

$cache = UserCache\Cache::getPool('clear-dtor-reentry');
$cache->clear();

seed_request_local_slot($cache, 'single', new ClearReentrantDtor());

ClearReentrantDtor::$armed = true;
var_dump($cache->clear());
ClearReentrantDtor::$armed = false;

var_dump($cache->has('single'));

foreach (['multi-1', 'multi-2', 'multi-3'] as $key) {
    seed_request_local_slot($cache, $key, new ClearReentrantDtor());
}

ClearReentrantDtor::$armed = true;
var_dump($cache->clear());
ClearReentrantDtor::$armed = false;

var_dump($cache->has('multi-1'));
var_dump($cache->has('multi-3'));

$cache = UserCache\Cache::getPool('clear-dtor-reentry');
seed_request_local_slot($cache, 'delete-pool', new DeletePoolReentrantDtor());

DeletePoolReentrantDtor::$armed = true;
var_dump(UserCache\Cache::deletePool('clear-dtor-reentry'));
DeletePoolReentrantDtor::$armed = false;

var_dump(UserCache\Cache::getPool('clear-dtor-reentry')->has('delete-pool'));

echo "done\n";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
done

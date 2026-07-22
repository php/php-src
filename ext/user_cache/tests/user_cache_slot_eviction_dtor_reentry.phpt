--TEST--
UserCache\Cache: request-local slot eviction may run destructors that re-enter the API
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class ReentrantDtor
{
    public int $n = 0;

    public function __destruct()
    {
        $cache = UserCache\Cache::getPool('slot-dtor');
        $cache->store('dtor-side', 'ran');
        $cache->fetch('dtor-side');
    }
}

$cache = UserCache\Cache::getPool('slot-dtor');
$cache->clear();

$obj = new ReentrantDtor();
$obj->n = 1;
var_dump($cache->store('obj', $obj));
unset($obj);

$first = $cache->fetch('obj');
var_dump($first->n);
unset($first);

$pid = pcntl_fork();
if ($pid === 0) {
    $child = new ReentrantDtor();
    $child->n = 2;
    UserCache\Cache::getPool('slot-dtor')->store('obj', $child);
    exit(0);
}

if ($pid > 0) {
    pcntl_waitpid($pid, $status);

    /* The generation moved, so this fetch evicts the stale request-local
     * slot, running the clone's destructor, which re-enters the cache. */
    $second = $cache->fetch('obj');
    var_dump($second->n);
    var_dump($cache->fetch('dtor-side'));
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
bool(true)
int(1)
int(2)
string(3) "ran"

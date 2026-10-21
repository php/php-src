--TEST--
UserCache\Cache: destructors run by the cycle collector or a WeakMap while the pool registry is trimmed cannot reach a freed pool and can register the pool being created
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=4M
memory_limit=512M
--FILE--
<?php
use UserCache\Cache;

final class ReachesDyingPool
{
    public $self;

    public function __destruct()
    {
        if (!isset($GLOBALS['observed'])) {
            $GLOBALS['observed'] = [count($GLOBALS['keep']), $GLOBALS['weak']->get()];
        }
    }
}

echo "pool free weak reference:\n";

$victim = UserCache\Cache::getPool('free-weak-reference');
$victim->clear();
$payload = [];
for ($i = 0; $i < 3000; $i++) {
    $payload[] = str_repeat('x', 100) . $i;
}
for ($k = 0; $k < 4; $k++) {
    $victim->store("big$k", $payload);
}
$held = [];
for ($k = 0; $k < 4; $k++) {
    $held[] = $victim->fetch("big$k");
}
$weak = WeakReference::create($victim);
$victim = null;
$keep = [];

gc_collect_cycles();
$status = gc_status();
for ($i = $status['threshold'] - $status['roots'] - 1; $i > 0; $i--) {
    $garbage = new ReachesDyingPool();
    $garbage->self = $garbage;
    unset($garbage);
}

for ($i = 0; $i < 16400; $i++) {
    $keep[] = UserCache\Cache::getPool("pool-$i");
}

[$poolsWhenCollected, $reached] = $observed;
echo 'collected while the registry was trimmed: ';
var_dump($poolsWhenCollected === 16383);
var_dump($reached);
var_dump($weak->get());
var_dump(count($held[2]));

unset($keep, $held);
gc_collect_cycles();

echo "\npool registry trim reentry:\n";

class RegistersPoolOnDestruct
{
    public function __destruct()
    {
        $GLOBALS['nested'] = Cache::getPool($GLOBALS['current']);
    }
}

$first = WeakReference::create(Cache::getPool('first'));

/* This pool is referenced only by the registry; freeing it releases its WeakMap value and runs the destructor. */
$hooks = new WeakMap;
$trap = Cache::getPool('trap');
$hooks[$trap] = new RegistersPoolOnDestruct;
$trapped = WeakReference::create($trap);
unset($trap);

for ($i = 0; $trapped->get() !== null && $i < 40000; $i++) {
    $GLOBALS['current'] = "tenant$i";
    $outer = Cache::getPool($GLOBALS['current']);
}

var_dump($first->get() === null, $outer === $GLOBALS['nested'], Cache::getPool($GLOBALS['current']) === $outer);
$entries = 0;
foreach (Cache::getPools() as $name => $pool) {
    $entries += $name === $GLOBALS['current'] ? 1 : 0;
}
var_dump($entries);
var_dump(Cache::deletePool($GLOBALS['current']), Cache::hasPool($GLOBALS['current']));
?>
--EXPECT--
pool free weak reference:
collected while the registry was trimmed: bool(true)
NULL
NULL
int(3000)

pool registry trim reentry:
bool(true)
bool(true)
bool(true)
int(1)
bool(true)
bool(false)

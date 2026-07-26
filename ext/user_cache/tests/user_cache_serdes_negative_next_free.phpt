--TEST--
UserCache\Cache: __sleep()/__wakeup() objects round-trip arrays whose next-free index is negative
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('serdes-negative-next-free');

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

class NegativeNextFreeBag
{
    public $arr;

    public function __sleep(): array
    {
        return ['arr'];
    }

    public function __wakeup(): void
    {
    }
}

/* The negative next-free index used to be rejected on decode, so the whole
 * fetch failed and the default was returned. */
$bag = new NegativeNextFreeBag();
$bag->arr = [-5 => 'v'];

ok('negative key store', $cache->store('bag', $bag) === true);

$fetched = $cache->fetch('bag', 'default');
ok('negative key fetch decoded', $fetched !== 'default');
ok('negative key class', $fetched instanceof NegativeNextFreeBag);
var_dump($fetched->arr);
$fetched->arr[] = 'appended';
ok('negative key append', array_keys($fetched->arr) === [-5, -4]);
ok('negative key value survived', $fetched->arr[-5] === 'v');

/* Same for an emptied array, where the next-free index is the only state. */
$emptied = [];
$emptied[-9] = 1;
unset($emptied[-9]);

$bag = new NegativeNextFreeBag();
$bag->arr = $emptied;

ok('emptied store', $cache->store('emptied', $bag) === true);

$fetched = $cache->fetch('emptied', 'default');
ok('emptied fetch decoded', $fetched !== 'default');
ok('emptied is empty', $fetched->arr === []);
$fetched->arr[] = 'appended';
ok('emptied append', array_keys($fetched->arr) === [-8]);
?>
--EXPECT--
negative key store: OK
negative key fetch decoded: OK
negative key class: OK
array(1) {
  [-5]=>
  string(1) "v"
}
negative key append: OK
negative key value survived: OK
emptied store: OK
emptied fetch decoded: OK
emptied is empty: OK
emptied append: OK

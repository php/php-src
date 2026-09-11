--TEST--
UserCache\Cache: __sleep()/__serialize() dropping the last reference to the stored object does not use it after free
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('sleep-last-ref');

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Holders reached through a plain object are re-read by the copy pass, so the
 * property the hook cleared is stored as null. Holders that snapshot their own
 * state keep the value alive in that snapshot. Either way the store must
 * complete instead of walking a freed object. */

class SleepPlainHolder
{
    public $inner;
}

class SleepSnapshotHolder
{
    public $inner;

    public function __sleep(): array
    {
        return ['inner'];
    }

    public function __wakeup(): void
    {
    }
}

class SleepDropper
{
    public static $holder;

    public $a = 1;

    public function __sleep(): array
    {
        /* Releases the only reference to $this while the store walks it. */
        SleepDropper::$holder->inner = null;

        return ['a'];
    }
}

class SerializePlainHolder
{
    public $inner;
}

class SerializeSnapshotHolder
{
    public $inner;

    public function __serialize(): array
    {
        return ['inner' => $this->inner];
    }

    public function __unserialize(array $data): void
    {
        $this->inner = $data['inner'];
    }
}

class SerializeDropper
{
    public static $holder;

    public $a = 1;

    public function __serialize(): array
    {
        SerializeDropper::$holder->inner = null;

        return ['a' => $this->a];
    }

    public function __unserialize(array $data): void
    {
        $this->a = $data['a'];
    }
}

$holder = new SleepPlainHolder();
$holder->inner = new SleepDropper();
SleepDropper::$holder = $holder;
ok('sleep plain store', $cache->store('sleep-plain', $holder) === true);
ok('sleep plain live property cleared', $holder->inner === null);
$fetched = $cache->fetch('sleep-plain');
ok('sleep plain fetched class', $fetched instanceof SleepPlainHolder);
ok('sleep plain fetched property', $fetched->inner === null);

$holder = new SleepSnapshotHolder();
$holder->inner = new SleepDropper();
SleepDropper::$holder = $holder;
ok('sleep snapshot store', $cache->store('sleep-snapshot', $holder) === true);
ok('sleep snapshot live property cleared', $holder->inner === null);
$fetched = $cache->fetch('sleep-snapshot');
ok('sleep snapshot fetched class', $fetched instanceof SleepSnapshotHolder);
ok('sleep snapshot inner class', $fetched->inner instanceof SleepDropper);
ok('sleep snapshot inner value', $fetched->inner->a === 1);

$holder = new SerializePlainHolder();
$holder->inner = new SerializeDropper();
SerializeDropper::$holder = $holder;
ok('serialize plain store', $cache->store('serialize-plain', $holder) === true);
ok('serialize plain live property cleared', $holder->inner === null);
$fetched = $cache->fetch('serialize-plain');
ok('serialize plain fetched class', $fetched instanceof SerializePlainHolder);
ok('serialize plain fetched property', $fetched->inner === null);

$holder = new SerializeSnapshotHolder();
$holder->inner = new SerializeDropper();
SerializeDropper::$holder = $holder;
ok('serialize snapshot store', $cache->store('serialize-snapshot', $holder) === true);
ok('serialize snapshot live property cleared', $holder->inner === null);
$fetched = $cache->fetch('serialize-snapshot');
ok('serialize snapshot fetched class', $fetched instanceof SerializeSnapshotHolder);
ok('serialize snapshot inner class', $fetched->inner instanceof SerializeDropper);
ok('serialize snapshot inner value', $fetched->inner->a === 1);

/* The pool survives every one of those stores. */
ok('cache still usable', $cache->store('scalar', 7) && $cache->fetch('scalar') === 7);

SleepDropper::$holder = null;
SerializeDropper::$holder = null;
unset($holder, $fetched);
gc_collect_cycles();
?>
--EXPECT--
sleep plain store: OK
sleep plain live property cleared: OK
sleep plain fetched class: OK
sleep plain fetched property: OK
sleep snapshot store: OK
sleep snapshot live property cleared: OK
sleep snapshot fetched class: OK
sleep snapshot inner class: OK
sleep snapshot inner value: OK
serialize plain store: OK
serialize plain live property cleared: OK
serialize plain fetched class: OK
serialize plain fetched property: OK
serialize snapshot store: OK
serialize snapshot live property cleared: OK
serialize snapshot fetched class: OK
serialize snapshot inner class: OK
serialize snapshot inner value: OK
cache still usable: OK

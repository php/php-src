--TEST--
UserCache\Cache: Serializable::serialize() dropping the last reference to the stored object does not use it after free
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('serializable-last-ref');

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* The legacy Serializable route builds its blob through the serdes encoder,
 * which runs serialize() while the store still walks the object graph. */

class SerPlainHolder
{
    public $inner;
}

class SerDropper implements Serializable
{
    public static $holder;

    public $a = 1;

    public function serialize(): string
    {
        /* Releases the only reference to $this while the encoder walks it. */
        SerDropper::$holder->inner = null;

        return (string) $this->a;
    }

    public function unserialize(string $data): void
    {
        $this->a = (int) $data;
    }
}

class SerSnapshotHolder implements Serializable
{
    public $inner;

    public function serialize(): string
    {
        return serialize($this->inner);
    }

    public function unserialize(string $data): void
    {
        $this->inner = unserialize($data);
    }
}

class SerDropper2 implements Serializable
{
    public static $holder;

    public $a = 7;

    public function serialize(): string
    {
        SerDropper2::$holder->inner = null;

        return (string) $this->a;
    }

    public function unserialize(string $data): void
    {
        $this->a = (int) $data;
    }
}

$holder = new SerPlainHolder();
$holder->inner = new SerDropper();
SerDropper::$holder = $holder;
ok('serializable plain store', $cache->store('serdes-plain', $holder) === true);
ok('serializable plain live property cleared', $holder->inner === null);
$fetched = $cache->fetch('serdes-plain');
ok('serializable plain fetched class', $fetched instanceof SerPlainHolder);
ok('serializable plain fetched property', $fetched->inner === null);

$holder = new SerSnapshotHolder();
$holder->inner = new SerDropper2();
SerDropper2::$holder = $holder;
ok('serializable snapshot store', $cache->store('serdes-snapshot', $holder) === true);
ok('serializable snapshot live property cleared', $holder->inner === null);
$fetched = $cache->fetch('serdes-snapshot');
ok('serializable snapshot fetched class', $fetched instanceof SerSnapshotHolder);
ok('serializable snapshot inner class', $fetched->inner instanceof SerDropper2);
ok('serializable snapshot inner value', $fetched->inner->a === 7);

/* The pool survives every one of those stores. */
ok('cache still usable', $cache->store('scalar', 5) && $cache->fetch('scalar') === 5);

SerDropper::$holder = null;
SerDropper2::$holder = null;
unset($holder, $fetched);
gc_collect_cycles();
?>
--EXPECT--
serializable plain store: OK
serializable plain live property cleared: OK
serializable plain fetched class: OK
serializable plain fetched property: OK
serializable snapshot store: OK
serializable snapshot live property cleared: OK
serializable snapshot fetched class: OK
serializable snapshot inner class: OK
serializable snapshot inner value: OK
cache still usable: OK

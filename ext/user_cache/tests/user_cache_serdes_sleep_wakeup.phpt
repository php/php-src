--TEST--
UserCache\Cache: __sleep()/__wakeup() classes round-trip through the serialization contract
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('serdes');

class SerdesSleeper
{
    public static int $sleepCalls = 0;
    public static int $wakeupCalls = 0;

    public string $kept = 'default-kept';
    public string $dropped = 'default-dropped';
    protected int $protectedKept = 0;
    private array $privateKept = [];

    public function __sleep(): array
    {
        self::$sleepCalls++;
        return ['kept', 'protectedKept', 'privateKept'];
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }

    public function describe(): string
    {
        return $this->kept . '/' . $this->dropped . '/' . $this->protectedKept . '/' . count($this->privateKept);
    }
}

$object = new SerdesSleeper();
$object->kept = 'stored-kept';
$object->dropped = 'stored-dropped';
(function () {
    $this->protectedKept = 7;
    $this->privateKept = ['x', 'y'];
})->call($object);

var_dump($cache->store('sleeper', $object));
var_dump(SerdesSleeper::$sleepCalls);

$fetched = $cache->fetch('sleeper');
var_dump($fetched instanceof SerdesSleeper);
var_dump(SerdesSleeper::$wakeupCalls);
var_dump($fetched->describe());

/* Restore hooks run on every fetch. */
$second = $cache->fetch('sleeper');
var_dump(SerdesSleeper::$wakeupCalls);
var_dump($second !== $fetched);

class SerdesBadSleeper
{
    public int $real = 1;

    public function __sleep(): array
    {
        return ['real', 'missing'];
    }

    public function __wakeup(): void
    {
    }
}

var_dump($cache->store('bad-sleeper', new SerdesBadSleeper()));
var_dump($cache->fetch('bad-sleeper')->real);

class SerdesWakerOnly
{
    public static int $wakeupCalls = 0;
    public int $value = 5;

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

$waker = new SerdesWakerOnly();
$waker->value = 11;
var_dump($cache->store('waker', $waker));
var_dump($cache->fetch('waker')->value);
var_dump(SerdesWakerOnly::$wakeupCalls);
?>
--EXPECTF--
bool(true)
int(1)
bool(true)
int(1)
string(31) "stored-kept/default-dropped/7/2"
int(2)
bool(true)

Warning: UserCache\Cache::store(): "missing" returned as member variable from __sleep() but does not exist in %s on line %d
bool(true)
int(1)
bool(true)
int(11)
int(1)

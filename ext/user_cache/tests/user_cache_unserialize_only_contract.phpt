--TEST--
UserCache\Cache: __unserialize() without __serialize() is honored
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class UnserializeOnlyContract
{
    public int $value = 7;
    public int $derived = 0;
    public static int $calls = 0;

    public function __unserialize(array $data): void
    {
        self::$calls++;
        $this->value = $data['value'];
        $this->derived = $this->value * 3;
    }
}

class SleepAndUnserializeContract
{
    public int $keep = 5;
    public int $drop = 99;
    public bool $wakeup = false;
    public array $keys = [];
    public static array $events = [];

    public function __sleep(): array
    {
        self::$events[] = 'sleep';

        return ['keep'];
    }

    public function __wakeup(): void
    {
        self::$events[] = 'wakeup';
        $this->wakeup = true;
    }

    public function __unserialize(array $data): void
    {
        self::$events[] = 'unserialize';
        $this->keys = array_keys($data);
        $this->keep = $data['keep'];
        $this->drop = -1;
    }
}

$cache = UserCache\Cache::getPool('unserialize-only-contract');

var_dump($cache->store('one', new UnserializeOnlyContract()));
$one = $cache->fetch('one');
var_dump($one->value, $one->derived, UnserializeOnlyContract::$calls);

var_dump($cache->store('two', new SleepAndUnserializeContract()));
$two = $cache->fetch('two');
var_dump($two->keep, $two->drop, $two->wakeup, $two->keys, SleepAndUnserializeContract::$events);
?>
--EXPECT--
bool(true)
int(7)
int(21)
int(1)
bool(true)
int(5)
int(-1)
bool(false)
array(1) {
  [0]=>
  string(4) "keep"
}
array(2) {
  [0]=>
  string(5) "sleep"
  [1]=>
  string(11) "unserialize"
}

--TEST--
UserCache\Cache: __sleep() objects restore from shared graph state
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('sleep-shared-graph');

class UCSleepSharedGraphBox
{
    public static int $sleepCalls = 0;
    public static int $wakeupCalls = 0;

    public string $kept = 'default-kept';
    public string $dropped = 'default-dropped';
    protected int $protectedKept = 0;
    private array $privateKept = [];
    public ?self $self = null;

    public function setHidden(int $protectedKept, array $privateKept): void
    {
        $this->protectedKept = $protectedKept;
        $this->privateKept = $privateKept;
    }

    public function __sleep(): array
    {
        self::$sleepCalls++;
        return ['kept', 'protectedKept', 'privateKept', 'self'];
    }

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
        $this->kept .= ':awake';
    }

    public function describe(): string
    {
        return $this->kept . '/' .
            $this->dropped . '/' .
            $this->protectedKept . '/' .
            count($this->privateKept) . '/' .
            ($this->self === $this ? 'self' : 'other');
    }
}

$box = new UCSleepSharedGraphBox();
$box->kept = 'stored-kept';
$box->dropped = 'stored-dropped';
$box->setHidden(7, ['x', 'y']);
$box->self = $box;

var_dump($cache->store('box', $box));
var_dump(UCSleepSharedGraphBox::$sleepCalls);

$first = $cache->fetch('box');
var_dump($first instanceof UCSleepSharedGraphBox);
var_dump(UCSleepSharedGraphBox::$wakeupCalls);
var_dump($first->describe());

$second = $cache->fetch('box');
var_dump(UCSleepSharedGraphBox::$wakeupCalls);
var_dump($second !== $first);
var_dump($second->self === $second);

class UCSleepSharedGraphReadonly
{
    public function __construct(public readonly string $token)
    {
    }

    public function __sleep(): array
    {
        return ['token'];
    }

    public function __wakeup(): void
    {
    }
}

var_dump($cache->store('readonly', new UCSleepSharedGraphReadonly('tok-1')));
var_dump($cache->fetch('readonly')->token);

class UCSleepSharedGraphWakeupMutates
{
    public array $items = [1];

    public function __sleep(): array
    {
        return ['items'];
    }

    public function __wakeup(): void
    {
        $this->items[] = 2;
    }
}

var_dump($cache->store('wakeup-mutates', new UCSleepSharedGraphWakeupMutates()));
$mutated = $cache->fetch('wakeup-mutates');
var_dump($mutated->items);
$mutated->items[] = 3;
var_dump($cache->fetch('wakeup-mutates')->items);

unset($box, $first, $second, $mutated);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
int(1)
bool(true)
int(1)
string(42) "stored-kept:awake/default-dropped/7/2/self"
int(2)
bool(true)
bool(true)
bool(true)
string(5) "tok-1"
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

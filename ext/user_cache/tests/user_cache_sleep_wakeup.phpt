--TEST--
UserCache\Cache: __sleep() and __wakeup() objects restore from shared graph state with per-fetch isolation
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* __sleep() selection, self-cycle and __wakeup() mutation isolated per fetch */
$cache = UserCache\Cache::getPool('sleep-wakeup-box');

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

UCSleepSharedGraphBox::$sleepCalls = 0;
UCSleepSharedGraphBox::$wakeupCalls = 0;

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

/* Release builds do not collect cycles at shutdown. */
unset($box, $first, $second, $mutated);
gc_collect_cycles();

/* __sleep() naming a missing member warns at store(); __wakeup()-only classes still wake */
$cache = UserCache\Cache::getPool('sleep-wakeup-missing-member');

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

SerdesWakerOnly::$wakeupCalls = 0;
$waker = new SerdesWakerOnly();
$waker->value = 11;
var_dump($cache->store('waker', $waker));
var_dump($cache->fetch('waker')->value);
var_dump(SerdesWakerOnly::$wakeupCalls);

/* Shared arrays and twice-referenced nodes inside a __sleep() graph */
$cache = UserCache\Cache::getPool('sleep-wakeup-graph');

class SerdesNode
{
    public ?SerdesNode $link = null;
    public array $items = [];

    public function __sleep(): array
    {
        return ['link', 'items'];
    }

    public function __wakeup(): void
    {
    }
}

$shared = ['payload' => range(1, 4)];
$a = new SerdesNode();
$b = new SerdesNode();
$a->items = $shared;
$b->items = $shared;
$a->link = $b;
var_dump($cache->store('pair', $a));
$pair = $cache->fetch('pair');
var_dump($pair->link instanceof SerdesNode);
var_dump($pair->items === $pair->link->items);

$node = new SerdesNode();
$node->items = ['x' => 1];
var_dump($cache->store('nested', ['first' => $node, 'second' => $node, 'plain' => [1, 2, 3]]));
$nested = $cache->fetch('nested');
var_dump($nested['first'] instanceof SerdesNode);
var_dump($nested['first'] === $nested['second']);
var_dump($nested['first']->items['x']);
var_dump($nested['plain']);

/* __wakeup()-only holder keeps identity with a child shared outside it */
$cache = UserCache\Cache::getPool('sleep-wakeup-holder');

class WakeupOnlyIdentityHolder
{
    public static int $wakeupCalls = 0;
    public object $child;

    public function __wakeup(): void
    {
        self::$wakeupCalls++;
    }
}

class WakeupOnlyIdentityChild
{
    public int $value = 1;
}

$child = new WakeupOnlyIdentityChild();
$holder = new WakeupOnlyIdentityHolder();
$holder->child = $child;

$native = unserialize(serialize(['holder' => $holder, 'child' => $child]));
var_dump($native['holder']->child === $native['child']);

WakeupOnlyIdentityHolder::$wakeupCalls = 0;

var_dump($cache->store('graph', ['holder' => $holder, 'child' => $child]));

$first = $cache->fetch('graph');
var_dump($first['holder']->child === $first['child']);
$first['holder']->child->value = 42;
var_dump($first['child']->value);
var_dump(WakeupOnlyIdentityHolder::$wakeupCalls);

$second = $cache->fetch('graph');
var_dump($second['holder']->child === $second['child']);
var_dump($second['child']->value);
var_dump(WakeupOnlyIdentityHolder::$wakeupCalls);
var_dump($first['holder'] !== $second['holder']);
?>
--EXPECTF--
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

Warning: UserCache\Cache::store(): "missing" returned as member variable from __sleep() but does not exist in %s on line %d
bool(true)
int(1)
bool(true)
int(11)
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
bool(true)
bool(true)
int(42)
int(1)
bool(true)
int(1)
int(2)
bool(true)

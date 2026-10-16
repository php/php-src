--TEST--
UserCache\Cache: private, protected, typed, readonly and inherited private properties restore through their declared slots
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
class Base
{
    private $secret = 'base-default';
    protected int $level = 0;

    public function setBase(string $secret, int $level): void
    {
        $this->secret = $secret;
        $this->level = $level;
    }

    public function base(): string
    {
        return $this->secret . '/' . $this->level;
    }
}

class Child extends Base
{
    private $secret = 'child-default';
    private ?Child $peer = null;
    protected array $tags = [];
    public readonly int $id;
    private int $ref = 0;
    private int $alias = 0;

    public function __construct(int $id)
    {
        $this->id = $id;
    }

    public function fill(string $secret, array $tags, ?Child $peer): void
    {
        $this->secret = $secret;
        $this->tags = $tags;
        $this->peer = $peer;
        $this->ref = 7;
        $this->alias = &$this->ref;
    }

    public function child(): string
    {
        return $this->secret . '/' . implode(',', $this->tags) . '/' . ($this->peer?->id ?? '-')
            . '/' . $this->ref . '/' . $this->alias;
    }

    public function bumpAlias(): int
    {
        $this->alias++;

        return $this->ref;
    }
}

class Sleeper extends Base
{
    private string $name = '';
    protected float $ratio = 0.0;
    private $skipped = 'skip';

    public function __construct(string $name, float $ratio)
    {
        $this->name = $name;
        $this->ratio = $ratio;
        $this->setBase("sleeper-$name", 3);
    }

    public function __sleep(): array
    {
        return ["\0Sleeper\0name", "\0*\0ratio", "\0Base\0secret", "\0*\0level"];
    }

    public function __wakeup(): void
    {
        echo "wakeup {$this->name}\n";
    }

    public function dump(): string
    {
        return $this->name . '/' . $this->ratio . '/' . $this->base() . '/' . $this->skipped;
    }
}

class Waker
{
    private int $count = 0;
    protected readonly string $label;

    public function __construct(string $label, int $count)
    {
        $this->label = $label;
        $this->count = $count;
    }

    public function __wakeup(): void
    {
        echo "wakeup {$this->label}\n";
    }

    public function dump(): string
    {
        return $this->label . '/' . $this->count;
    }
}

$cache = UserCache\Cache::getPool('graph-declared-properties');
$cache->clear();

$first = new Child(1);
$second = new Child(2);
$first->setBase('first-base', 10);
$first->fill('first', ['a', 'b'], $second);
$second->fill('second', [], $first);
var_dump($cache->store('children', [$first, $second]));
[$a, $b] = $cache->fetch('children');
var_dump($a->child(), $a->base(), $b->child(), $b->base());
var_dump($a->id, $b->id, $a->child() === $first->child());
var_dump($a->bumpAlias(), $a->child());
try {
    $a->id = 5;
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
var_dump(serialize($cache->fetch('children')) === serialize([$first, $second]));

var_dump($cache->store('sleepers', [new Sleeper('x', 0.5), new Sleeper('y', 1.5)]));
foreach ($cache->fetch('sleepers') as $sleeper) {
    var_dump($sleeper->dump());
}

var_dump($cache->store('wakers', [new Waker('w1', 1), new Waker('w2', 2)]));
foreach ($cache->fetch('wakers') as $waker) {
    var_dump($waker->dump());
}

/* Uninitialized-property flags match native unserialize(). */
class Lazy
{
    public int $pub;
    private int $priv;
    protected int $prot;

    public function __construct()
    {
        $this->pub = 1;
        $this->priv = 2;
        $this->prot = 3;
    }

    public function __get($name)
    {
        return "get:$name";
    }

    public function probe(): string
    {
        $out = [];
        foreach (['pub', 'priv', 'prot'] as $name) {
            unset($this->$name);
            try {
                $out[] = $this->$name;
            } catch (Error $e) {
                $out[] = $e->getMessage();
            }
        }

        return implode("\n", $out);
    }
}

$lazy = new Lazy();
$cache->store('lazy', [$lazy, $lazy]);
var_dump($cache->fetch('lazy')[0]->probe() === unserialize(serialize($lazy))->probe());

/* Release builds do not collect cycles at shutdown, and the child exits first. */
$first = $second = $a = $b = null;
gc_collect_cycles();

/* A class whose property type changed after the store fails like unserialize(). */
$pid = pcntl_fork();
if ($pid === 0) {
    eval('class Retyped { private string $value = "text"; }');
    exit(UserCache\Cache::getPool('graph-declared-properties')->store('retyped', [new Retyped(), new Retyped()]) ? 0 : 1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
eval('class Retyped { private int $value = 1; }');
try {
    var_dump($cache->fetch('retyped'));
} catch (TypeError $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
try {
    unserialize('O:7:"Retyped":1:{s:14:"' . "\0Retyped\0value" . '";s:4:"text";}');
} catch (TypeError $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
string(15) "first/a,b/2/7/7"
string(13) "first-base/10"
string(13) "second//1/7/7"
string(14) "base-default/0"
int(1)
int(2)
bool(true)
int(8)
string(15) "first/a,b/2/8/8"
Error: Cannot modify readonly property Child::$id
bool(true)
bool(true)
wakeup x
wakeup y
string(22) "x/0.5/sleeper-x/3/skip"
string(22) "y/1.5/sleeper-y/3/skip"
bool(true)
wakeup w1
wakeup w2
string(4) "w1/1"
string(4) "w2/2"
bool(true)
bool(true)
TypeError: Cannot assign string to property Retyped::$value of type int
TypeError: Cannot assign string to property Retyped::$value of type int

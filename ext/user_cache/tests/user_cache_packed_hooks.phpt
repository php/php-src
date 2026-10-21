--TEST--
UserCache\Cache: packed arrays follow store and decode hook mutations and keep late hook objects
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* Sibling store hook swaps a packed element to a hooked object after sizing */
class PreviouslySizedPackedNodeWithANameLongEnoughForTheReplacementStateNode
{
}

class PackedLateHolder
{
    public array $values;
    public mixed $trigger;
}

class PackedLateTrigger
{
    public static ?PackedLateHolder $holder;
    public static int $position;
    public static bool $replace;
    public static array $calls;

    public function __serialize(): array
    {
        self::$calls[] = 'calc';
        self::$holder->values[self::$position] = new PackedLateState;
        return [];
    }

    public function __unserialize(array $state): void
    {
    }
}

class PackedLateState
{
    public function __serialize(): array
    {
        PackedLateTrigger::$calls[] = 'copy';
        if (PackedLateTrigger::$replace) {
            PackedLateTrigger::$holder->values[63] = 900;
        }
        return [];
    }

    public function __unserialize(array $state): void
    {
    }
}

$cache = UserCache\Cache::getPool('packed-hooks-late');
foreach ([0, 32] as $position) {
    foreach ([false, true] as $replace) {
        $holder = new PackedLateHolder;
        $holder->values = range(0, 63);
        $holder->values[0] = new stdClass;
        $holder->values[$position] = new PreviouslySizedPackedNodeWithANameLongEnoughForTheReplacementStateNode;
        $holder->trigger = new PackedLateTrigger;
        PackedLateTrigger::$holder = $holder;
        PackedLateTrigger::$position = $position;
        PackedLateTrigger::$replace = $replace;
        PackedLateTrigger::$calls = [];
        if (!$cache->store('value', $holder)) {
            throw new RuntimeException('store failed');
        }
        $fetched = $cache->fetch('value');
        var_dump(
            array_is_list($fetched->values),
            $fetched->values[$position] instanceof PackedLateState,
            $fetched->values[63] === ($replace ? 900 : 63),
            PackedLateTrigger::$calls === ['calc', 'copy']
        );
    }
}
PackedLateTrigger::$holder = null;

/* Sibling store hook unsets and adds keys on sized and not yet sized packed arrays */
class PackedStoreHolder
{
    public array $values;
    public mixed $trigger = null;
}

class PackedStoreMutator
{
    public static ?PackedStoreHolder $holder = null;
    public static string $operation;
    public static int $calls = 0;

    public function __serialize(): array
    {
        self::$calls++;
        unset(self::$holder->values[1]);
        if (self::$operation === 'mixed') {
            self::$holder->values['changed'] = 19;
        }

        return ['changed' => 19];
    }

    public function __unserialize(array $state): void
    {
    }
}

$cache = UserCache\Cache::getPool('packed-hooks-store');
foreach (['current', 'earlier'] as $position) {
    foreach (['hole', 'mixed'] as $operation) {
        $holder = new PackedStoreHolder;
        $holder->values = range(0, 63);
        if ($position === 'current') {
            $holder->values[] = new PackedStoreMutator;
        } else {
            $holder->values[0] = new stdClass;
            $holder->trigger = new PackedStoreMutator;
        }
        PackedStoreMutator::$holder = $holder;
        PackedStoreMutator::$operation = $operation;
        PackedStoreMutator::$calls = 0;

        if (!$cache->store('value', $holder)) {
            throw new RuntimeException("$position $operation store failed");
        }
        $fetched = $cache->fetch('value');
        var_dump(array_keys($fetched->values) === array_keys($holder->values));
        var_dump($fetched->values[63] === 63 && !isset($fetched->values[1]));
        var_dump($operation !== 'mixed' || $fetched->values['changed'] === 19);
        var_dump(PackedStoreMutator::$calls === 1);
        echo "$position $operation\n";
    }
}

/* Release builds do not collect cycles at shutdown. */
PackedStoreMutator::$holder = null;
unset($holder, $fetched);
gc_collect_cycles();

/* Decode hook mutates the parent packed array and keeps its internal cursor */
class PackedMutationHolder
{
    public array $values;
}

class PackedMutationProbe
{
    public static array $prefixes = [];
    public array $parent;
    public string $operation;

    public function __serialize(): array
    {
        return ['parent' => &$this->parent, 'operation' => $this->operation];
    }

    public function __unserialize(array $state): void
    {
        $this->parent =& $state['parent'];
        $this->operation = $state['operation'];
        self::$prefixes[] = array_keys($this->parent);

        switch ($this->operation) {
            case 'mixed':
                $this->parent['hook'] = 'mixed';
                break;
            case 'holes':
                unset($this->parent[1]);
                break;
            case 'append':
                for ($i = 0; $i < 12; $i++) {
                    $this->parent[] = 100 + $i;
                }
                break;
            case 'replace':
                $this->parent = array_fill(0, 1, 111);
                break;
            case 'cursor':
                next($this->parent);
                break;
        }
    }
}

$cache = UserCache\Cache::getPool('packed-hooks-decode');
foreach (['mixed', 'holes', 'append', 'replace', 'cursor'] as $operation) {
    $values = [10, 20, 30];
    $probe = new PackedMutationProbe;
    $probe->parent =& $values;
    $probe->operation = $operation;
    $values[] = $probe;
    $values[] = 40;
    $values[] = 50;
    $holder = new PackedMutationHolder;
    $holder->values =& $values;
    if (!$cache->store($operation, $holder)) {
        throw new RuntimeException('store failed');
    }

    $native = unserialize(serialize($holder));
    $restored = $cache->fetch($operation);
    echo $operation, ':', implode(',', array_keys($restored->values)), ':',
        key($restored->values), "\n";
    var_dump(array_keys($restored->values) === array_keys($native->values));
    if ($operation === 'replace') {
        var_dump($restored->values === [111]);
    } else {
        $restoredProbe = $restored->values[3];
        var_dump($restoredProbe->parent[3] === $restoredProbe
            && $restored->values[4] === 40 && $restored->values[5] === 50);
    }

    /* Release builds do not collect cycles at shutdown. */
    unset($restoredProbe, $restored, $native, $holder, $probe, $values);
    gc_collect_cycles();
}

var_dump(PackedMutationProbe::$prefixes === array_fill(0, 10, [0, 1, 2, 3, 4, 5]));

/* The complete graph is visible to the restore hook and survives its exception. */
class PackedDecodePrefixProbe
{
    public static bool $throw = false;
    public static array $prefixes = [];
    public array $parent;

    public function __serialize(): array
    {
        return ['parent' => &$this->parent];
    }

    public function __unserialize(array $state): void
    {
        $this->parent =& $state['parent'];
        self::$prefixes[] = array_keys($this->parent);
        $nested = UserCache\Cache::getPool('packed-hooks-prefix')->fetch('nested');
        if ($nested[0] !== 7 || !($nested[1] instanceof stdClass)) {
            throw new RuntimeException('reentrant fetch failed');
        }
        if (self::$throw) {
            throw new RuntimeException('restore failed');
        }
    }
}

$cache = UserCache\Cache::getPool('packed-hooks-prefix');
$cache->store('nested', [7, new stdClass]);
$values = [1, 'two', false];
$probe = new PackedDecodePrefixProbe;
$probe->parent =& $values;
$values[] = $probe;
$values[] = 'after';
$values[] = 6;
var_dump($cache->store('value', $values));

PackedDecodePrefixProbe::$throw = true;
try {
    $cache->fetch('value');
} catch (RuntimeException $exception) {
    echo $exception->getMessage(), "\n";
}
PackedDecodePrefixProbe::$throw = false;
$result = $cache->fetch('value');
var_dump($result[0], $result[1], $result[2], $result[4], $result[5]);
var_dump($result[3] === $result[3]->parent[3]);
foreach (PackedDecodePrefixProbe::$prefixes as $prefix) {
    echo implode(',', $prefix), "\n";
}
$result[] = 7;
echo array_key_last($result), "\n";

$result = $cache->fetch('value');
echo count($result), "\n";

/* Release builds do not collect cycles at shutdown. */
unset($probe, $values, $result, $exception);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
current hole
bool(true)
bool(true)
bool(true)
bool(true)
current mixed
bool(true)
bool(true)
bool(true)
bool(true)
earlier hole
bool(true)
bool(true)
bool(true)
bool(true)
earlier mixed
mixed:0,1,2,3,4,5,hook:0
bool(true)
bool(true)
holes:0,2,3,4,5:0
bool(true)
bool(true)
append:0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17:0
bool(true)
bool(true)
replace:0:0
bool(true)
bool(true)
cursor:0,1,2,3,4,5:1
bool(true)
bool(true)
bool(true)
bool(true)
restore failed
int(1)
string(3) "two"
bool(false)
string(5) "after"
int(6)
bool(true)
0,1,2,3,4,5
0,1,2,3,4,5
6
6

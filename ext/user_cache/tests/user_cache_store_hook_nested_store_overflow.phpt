--TEST--
UserCache\Cache: a store() run by a __serialize() hook of an outer store() neither hides nor forges the outer value's nesting failure
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=32M
memory_limit=256M
--FILE--
<?php
const DEPTH = 5000;

/* Fiber stacks give a stack limit that does not depend on the process stack. */
function in_fiber(string $stackSize, callable $callback): mixed
{
    ini_set('fiber.stack_size', $stackSize);
    $fiber = new Fiber($callback);
    $fiber->start();

    return $fiber->getReturn();
}

function chain(int $depth): stdClass
{
    $node = null;
    for ($i = 0; $i < $depth; $i++) {
        $next = new stdClass();
        $next->depth = $i;
        $next->next = $node;
        $node = $next;
    }

    return $node;
}

function chain_depth(mixed $node): int
{
    $depth = 0;
    while ($node instanceof stdClass) {
        $node = $node->next;
        $depth++;
    }

    return $depth;
}

class NestedStorer
{
    public static ?UserCache\Cache $cache = null;
    public static mixed $inner = null;

    public function __construct(public int $id)
    {
    }

    public function __serialize(): array
    {
        try {
            $stored = var_export(self::$cache->store("inner-{$this->id}", self::$inner), true);
        } catch (TypeError $e) {
            $stored = $e->getMessage();
        }
        echo "  inner store: $stored\n";

        return ['id' => $this->id];
    }

    public function __unserialize(array $data): void
    {
        $this->id = $data['id'];
    }
}

function outer_store(UserCache\Cache $cache, string $key, mixed $value, string $stackSize): string
{
    return in_fiber($stackSize, function () use ($cache, $key, $value) {
        try {
            return var_export($cache->store($key, $value), true);
        } catch (TypeError $e) {
            return $e->getMessage();
        }
    });
}

$cache = UserCache\Cache::getPool('nested-store-overflow');
$cache->clear();
NestedStorer::$cache = $cache;

$deep = chain(DEPTH);
$small = ['a' => [1, 2]];

echo "inner overflows, outer overflows:\n";
NestedStorer::$inner = $deep;
echo outer_store($cache, 'outer-1', [new NestedStorer(1), $deep], '512K'), "\n";
var_dump($cache->has('outer-1'), $cache->has('inner-1'));

echo "inner overflows, outer fits:\n";
echo outer_store($cache, 'outer-2', [new NestedStorer(2), $small], '512K'), "\n";
var_dump($cache->has('inner-2'));
$fetched = $cache->fetch('outer-2');
var_dump($fetched[0] instanceof NestedStorer && $fetched[0]->id === 2 && $fetched[1] === $small);

echo "inner fits, outer overflows:\n";
NestedStorer::$inner = $small;
echo outer_store($cache, 'outer-3', [new NestedStorer(3), $deep], '512K'), "\n";
var_dump($cache->has('outer-3'), $cache->fetch('inner-3') === $small);

echo "outer overflows before the hook runs:\n";
echo outer_store($cache, 'outer-4', [$deep, new NestedStorer(4)], '512K'), "\n";
var_dump($cache->has('outer-4'), $cache->has('inner-4'));

echo "both fit on a larger stack:\n";
NestedStorer::$inner = $deep;
echo outer_store($cache, 'outer-5', [new NestedStorer(5), $deep], '32M'), "\n";
var_dump(in_fiber('32M', fn () => chain_depth($cache->fetch('inner-5'))));
var_dump(in_fiber('32M', fn () => chain_depth($cache->fetch('outer-5')[1])));

echo "the cache stays usable afterwards:\n";
var_dump($cache->store('after', 'ok'), $cache->fetch('after'));
var_dump(error_get_last());

/* Destroying a deep value recurses once per level: release the cached prototypes on a large fiber
 * stack and dismantle the local chain from the outside in. */
in_fiber('32M', fn () => $cache->clear());
unset($fetched);
while ($deep instanceof stdClass) {
    $deep = $deep->next;
}
?>
--EXPECT--
inner overflows, outer overflows:
  inner store: Value is nested too deeply to be stored in the user cache
Value is nested too deeply to be stored in the user cache
bool(false)
bool(false)
inner overflows, outer fits:
  inner store: Value is nested too deeply to be stored in the user cache
true
bool(false)
bool(true)
inner fits, outer overflows:
  inner store: true
Value is nested too deeply to be stored in the user cache
bool(false)
bool(true)
outer overflows before the hook runs:
Value is nested too deeply to be stored in the user cache
bool(false)
bool(false)
both fit on a larger stack:
  inner store: true
true
int(5000)
int(5000)
the cache stays usable afterwards:
bool(true)
string(2) "ok"
NULL

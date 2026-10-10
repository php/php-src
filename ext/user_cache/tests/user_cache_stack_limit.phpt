--TEST--
UserCache\Cache: values nested beyond the C stack limit are refused at store() and fall back to the default at fetch()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=32M
memory_limit=256M
--FILE--
<?php
const DEPTH = 5000;

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

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

class DtorProbe
{
    public static ?Closure $onDestruct = null;

    public function __destruct()
    {
        if (self::$onDestruct !== null) {
            (self::$onDestruct)();
        }
    }
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

$cache = UserCache\Cache::getPool('stack-limit');
$cache->clear();
$cache->store('small', ['a' => [1, 2]]);

/* Graphs are built and released on the process stack; only the cache calls run in fibers. */
$graph = chain(DEPTH);

/* store() refuses a graph that does not fit the remaining stack without a partial store */
$refused = in_fiber('512K', function () use ($cache, $graph) {
    try {
        return $cache->store('refused', $graph) ? 'stored' : 'false';
    } catch (TypeError $e) {
        return $e->getMessage();
    }
});
echo $refused, "\n";
var_dump($cache->has('refused'));

/* The same graph is storable and fetchable with a larger stack */
$stored = in_fiber('32M', fn () => $cache->store('deep', $graph));
var_dump($stored);
var_dump(in_fiber('32M', fn () => chain_depth($cache->fetch('deep'))));

/* A decode that runs out of stack yields the default and keeps the entry */
$results = in_fiber('512K', function () use ($cache) {
    $out = [];
    $out['fetch'] = $cache->fetch('deep', 'DEFAULT');
    $out['fetch-after'] = $cache->fetch('small');
    $out['multi'] = $cache->fetchMultiple(['small', 'deep', 'missing'], 'DEFAULT');
    $out['has'] = $cache->has('deep');

    return $out;
});
ok('fetch default', $results['fetch'] === 'DEFAULT');
ok('next fetch unaffected', $results['fetch-after'] === ['a' => [1, 2]]);
ok('fetchMultiple default only for the deep key', $results['multi'] === [
    'small' => ['a' => [1, 2]],
    'deep' => 'DEFAULT',
    'missing' => 'DEFAULT',
]);
ok('entry kept', $results['has'] && $cache->has('deep'));
var_dump(error_get_last());
var_dump(in_fiber('32M', fn () => chain_depth($cache->fetch('deep'))));

echo "exception from a destructor of the partial value:\n";
$probe = new DtorProbe();
var_dump(in_fiber('32M', fn () => $cache->store('deep-dtor', ['probe' => new DtorProbe(), 'deep' => $graph])));
var_dump(in_fiber('32M', fn () => $cache->store('deep-dtor-shared', ['probe' => $probe, 'again' => $probe, 'deep' => $graph])));
unset($probe);
DtorProbe::$onDestruct = static function (): void {
    throw new RuntimeException('thrown by a destructor');
};
$results = in_fiber('512K', function () use ($cache) {
    $calls = [
        'fetch' => fn () => $cache->fetch('deep-dtor', 'DEFAULT'),
        'fetchMultiple' => fn () => $cache->fetchMultiple(['small', 'deep-dtor', 'missing'], 'DEFAULT'),
        'remember' => fn () => $cache->remember('deep-dtor', function () {
            echo "remember callback ran\n";

            return 'computed';
        }),
    ];
    $out = [];
    foreach ($calls as $method => $call) {
        try {
            $out[] = $method . ': ' . var_export($call(), true);
        } catch (RuntimeException $e) {
            $out[] = $method . ': ' . get_class($e) . ': ' . $e->getMessage();
        }
    }

    return $out;
});
DtorProbe::$onDestruct = null;
echo implode("\n", $results), "\n";
ok('entry kept after destructor exceptions', $cache->has('deep-dtor'));

echo "cache calls from a destructor of the partial value:\n";
DtorProbe::$onDestruct = static function () use ($cache): void {
    $cache->has('small');
    $cache->fetch('small');
};
$results = in_fiber('512K', fn () => [
    'fetch' => $cache->fetch('deep-dtor', 'DEFAULT'),
    'fetchMultiple' => $cache->fetchMultiple(['deep-dtor'], 'DEFAULT'),
    'fetch shared' => $cache->fetch('deep-dtor-shared', 'DEFAULT'),
    'fetchMultiple shared' => $cache->fetchMultiple(['deep-dtor-shared'], 'DEFAULT'),
]);
DtorProbe::$onDestruct = null;
ok('defaults returned', $results === [
    'fetch' => 'DEFAULT',
    'fetchMultiple' => ['deep-dtor' => 'DEFAULT'],
    'fetch shared' => 'DEFAULT',
    'fetchMultiple shared' => ['deep-dtor-shared' => 'DEFAULT'],
]);
ok('entries kept after nested cache calls', $cache->has('deep-dtor') && $cache->has('deep-dtor-shared'));

/* Nested arrays either store or throw; no depth fails without an exception. */
$nested = [];
for ($depth = 500; $depth <= 20000; $depth += 500) {
    $arr = [];
    for ($i = 0; $i < $depth; $i++) {
        $arr = [$arr];
    }
    $nested[$depth] = $arr;
}
$silent = in_fiber('512K', function () use ($cache, $nested) {
    $silent = [];
    foreach ($nested as $depth => $arr) {
        try {
            if (!$cache->store("nested-$depth", $arr)) {
                $silent[] = $depth;
            }
        } catch (TypeError $e) {
        }
    }
    return $silent;
});
var_dump($silent);

/* Destroying a deep value recurses once per level: release the cached prototypes on a large fiber
 * stack and dismantle local values from the outside in. */
in_fiber('32M', fn () => $cache->clear());
foreach (array_keys($nested) as $depth) {
    $arr = $nested[$depth];
    unset($nested[$depth]);
    while ($arr !== []) {
        $arr = $arr[0];
    }
}
while ($graph instanceof stdClass) {
    $graph = $graph->next;
}
?>
--EXPECT--
Value is nested too deeply to be stored in the user cache
bool(false)
bool(true)
int(5000)
fetch default: OK
next fetch unaffected: OK
fetchMultiple default only for the deep key: OK
entry kept: OK
NULL
int(5000)
exception from a destructor of the partial value:
bool(true)
bool(true)
fetch: RuntimeException: thrown by a destructor
fetchMultiple: RuntimeException: thrown by a destructor
remember: RuntimeException: thrown by a destructor
entry kept after destructor exceptions: OK
cache calls from a destructor of the partial value:
defaults returned: OK
entries kept after nested cache calls: OK
array(0) {
}

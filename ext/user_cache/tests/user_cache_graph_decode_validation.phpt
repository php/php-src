--TEST--
UserCache\Cache: decoding bounds graph nesting, validates verbatim arrays once per request and keeps resolve caches per node kind
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=64M
memory_limit=512M
--FILE--
<?php
/* Mirrors UCACHE_DECODE_MAX_DEPTH in user_cache_shared_graph_decode.c; the counter only exists on builds
 * without the engine's C stack check (ZEND_CHECK_STACK_LIMIT), whose presence zend.max_allowed_stack_size reveals. */
const MAX_DEPTH = 8192;
define('COUNTS_NESTING', ini_get('zend.max_allowed_stack_size') === false);

function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Fiber stacks keep the C stack out of the picture so only the decoder's own depth limit applies. */
function in_fiber(callable $callback): mixed
{
    ini_set('fiber.stack_size', '64M');
    $fiber = new Fiber($callback);
    $fiber->start();

    return $fiber->getReturn();
}

function nest(mixed $inner, int $depth): array
{
    for ($i = 0; $i < $depth; $i++) {
        $inner = [$inner];
    }

    return $inner;
}

function nesting(mixed $val): int
{
    $depth = 0;
    while (is_array($val) && $val !== []) {
        $val = $val[0];
        $depth++;
    }

    return $depth;
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
        $node = $node->next ?? null;
        $depth++;
    }

    return $depth;
}

/* Destroying a deep value recurses once per level: dismantle from the outside in. */
function dismantle(mixed &$val): void
{
    while (is_array($val) && $val !== []) {
        $val = $val[0];
    }
    while ($val instanceof stdClass) {
        $val = $val->next ?? null;
    }
}

function check_depth_limit(UserCache\Cache $cache, string $label, mixed $fits, mixed $deep, callable $measure, int $expected): void
{
    ok("$label stores", in_fiber(fn () => $cache->store("$label-fits", $fits) && $cache->store("$label-deep", $deep)));

    $fetched = in_fiber(fn () => $cache->fetch("$label-fits"));
    ok("$label at the limit decodes", $measure($fetched) === $expected);
    dismantle($fetched);

    $fetched = in_fiber(fn () => $cache->fetch("$label-fits"));
    ok("$label at the limit decodes again", $measure($fetched) === $expected);
    dismantle($fetched);

    $result = in_fiber(fn () => $cache->fetch("$label-deep", 'DEFAULT'));
    ok("$label beyond the limit yields the default or decodes without the counter", COUNTS_NESTING ? $result === 'DEFAULT' : $measure($result) === $expected + 1);
    ok("$label beyond the limit keeps the entry", $cache->has("$label-deep"));
    ok("$label beyond the limit is silent", error_get_last() === null);
    ok("$label next fetch unaffected", $cache->fetch('control') === ['control' => true]);
    dismantle($result);
}

$cache = UserCache\Cache::getPool('graph-decode-validation');
$cache->clear();
$cache->store('control', ['control' => true]);

echo "nesting depth limit:\n";

/* Verbatim (zero-copy) arrays are walked by the validator with the same limit. */
$fits = nest([], MAX_DEPTH);
$deep = nest([], MAX_DEPTH + 1);
check_depth_limit($cache, 'verbatim', $fits, $deep, 'nesting', MAX_DEPTH);
dismantle($fits);
dismantle($deep);

/* An object at the bottom makes the arrays dynamic nodes; the object itself is one level. */
$fits = nest(new stdClass(), MAX_DEPTH - 1);
$deep = nest(new stdClass(), MAX_DEPTH);
check_depth_limit($cache, 'dynamic', $fits, $deep, 'nesting', MAX_DEPTH - 1);
dismantle($fits);
dismantle($deep);

$fits = chain(MAX_DEPTH);
$deep = chain(MAX_DEPTH + 1);
check_depth_limit($cache, 'objects', $fits, $deep, 'chain_depth', MAX_DEPTH);
dismantle($fits);
dismantle($deep);

echo "resolve cache kinds:\n";

enum Suit: string
{
    case Hearts = 'H';
    case Spades = 'S';
}

class Wakes
{
    public int $n = 1;
    public bool $woke = false;

    public function __wakeup(): void
    {
        $this->woke = true;
    }
}

class Sleeps
{
    public int $a = 1;
    public int $b = 2;

    public function __sleep(): array
    {
        return ['a'];
    }
}

class Plain
{
    public int $n = 3;
}

$kinds = [
    's' => Suit::Hearts,
    'w' => new Wakes(),
    'p' => new Sleeps(),
    'plain' => new Plain(),
    't' => Suit::Spades,
    'leaf' => [1, 2],
    'again' => [Suit::Hearts, new Wakes(), new Sleeps()],
];
ok('kinds store', $cache->store('kinds', $kinds));
for ($i = 0; $i < 3; $i++) {
    $v = $cache->fetch('kinds');
    ok("kinds fetch $i", $v['s'] === Suit::Hearts && $v['t'] === Suit::Spades
        && $v['w'] instanceof Wakes && $v['w']->woke && $v['w']->n === 1
        && $v['p'] instanceof Sleeps && $v['p']->a === 1
        && $v['plain'] instanceof Plain && $v['plain']->n === 3
        && $v['leaf'] === [1, 2]
        && $v['again'][0] === Suit::Hearts && $v['again'][1]->woke && $v['again'][2]->a === 1);
}

echo "verbatim arrays:\n";

$leaf = ['x' => 1.5, 'y' => "str\0ing", 'z' => [true, null, false], 'e' => []];
$holes = [1, 2, 3, 4];
unset($holes[1], $holes[2]);
$packedHoles = range(0, 15);
unset($packedHoles[3]);
$matrix = [
    'packed' => [1, 2, 3, 'four', 5.0, null, true, false, []],
    'hash' => ['a' => 1, 'b' => 'two', 7 => 'seven', -3 => 'minus', '' => 'empty key', 'nested' => ['k' => 'v']],
    'shared' => [$leaf, $leaf, ['inner' => $leaf, 'twice' => $leaf]],
    'holes' => $holes,
    'packed holes' => $packedHoles,
    'large' => range(1, 100000),
    'strings' => array_fill(0, 64, str_repeat('s', 100)),
];
foreach ($matrix as $label => $value) {
    ok("$label store", $cache->store($label, $value));
    ok("$label first fetch", $cache->fetch($label) === $value);
    ok("$label second fetch", $cache->fetch($label) === $value);
}
ok('matrix store', $cache->store('matrix', $matrix));
ok('matrix first fetch', $cache->fetch('matrix') === $matrix);
ok('matrix second fetch', $cache->fetch('matrix') === $matrix);
ok('matrix multiple', $cache->fetchMultiple(['matrix', 'control']) === ['matrix' => $matrix, 'control' => ['control' => true]]);
?>
--EXPECT--
nesting depth limit:
verbatim stores: OK
verbatim at the limit decodes: OK
verbatim at the limit decodes again: OK
verbatim beyond the limit yields the default or decodes without the counter: OK
verbatim beyond the limit keeps the entry: OK
verbatim beyond the limit is silent: OK
verbatim next fetch unaffected: OK
dynamic stores: OK
dynamic at the limit decodes: OK
dynamic at the limit decodes again: OK
dynamic beyond the limit yields the default or decodes without the counter: OK
dynamic beyond the limit keeps the entry: OK
dynamic beyond the limit is silent: OK
dynamic next fetch unaffected: OK
objects stores: OK
objects at the limit decodes: OK
objects at the limit decodes again: OK
objects beyond the limit yields the default or decodes without the counter: OK
objects beyond the limit keeps the entry: OK
objects beyond the limit is silent: OK
objects next fetch unaffected: OK
resolve cache kinds:
kinds store: OK
kinds fetch 0: OK
kinds fetch 1: OK
kinds fetch 2: OK
verbatim arrays:
packed store: OK
packed first fetch: OK
packed second fetch: OK
hash store: OK
hash first fetch: OK
hash second fetch: OK
shared store: OK
shared first fetch: OK
shared second fetch: OK
holes store: OK
holes first fetch: OK
holes second fetch: OK
packed holes store: OK
packed holes first fetch: OK
packed holes second fetch: OK
large store: OK
large first fetch: OK
large second fetch: OK
strings store: OK
strings first fetch: OK
strings second fetch: OK
matrix store: OK
matrix first fetch: OK
matrix second fetch: OK
matrix multiple: OK

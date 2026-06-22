--TEST--
OPcache volatile cache: __serialize objects use a transparent shared-graph node (called once), Serializable throws, object identity and cycles are preserved
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\VolatileCache as VC;

class Hooked
{
    public static int $ser = 0;
    public static int $unser = 0;

    public function __construct(public int $v = 0) {}

    public function __serialize(): array
    {
        self::$ser++;

        return ['v' => $this->v];
    }

    public function __unserialize(array $data): void
    {
        self::$unser++;
        $this->v = $data['v'];
    }
}

class Box
{
    public ?Hooked $h = null;
    public string $tag = 't';
}

/* A nested __serialize object keeps the whole graph on the fast path (not an
 * opaque serializer fallback), and __serialize() runs exactly once on store. */
$box = new Box();
$box->h = new Hooked(42);
Hooked::$ser = 0;
VC::getInstance('default')->store('k_box', $box);
echo "serialize calls: ", Hooked::$ser, "\n";

/* __unserialize runs once per request (the prototype is then cloned), and each
 * fetch is an independent instance. */
Hooked::$unser = 0;
$a = VC::getInstance('default')->fetch('k_box');
$b = VC::getInstance('default')->fetch('k_box');
echo "v={$a->h->v} tag={$a->tag}\n";
echo "unserialize calls: ", Hooked::$unser, "\n";
var_dump($a->h !== $b->h);

/* Objects implementing the deprecated Serializable interface (without
 * __serialize) are rejected. */
class Legacy implements Serializable
{
    public function serialize(): string { return ''; }
    public function unserialize(string $data): void {}
}

try {
    VC::getInstance('default')->store('k_legacy', new Legacy());
    echo "legacy: stored\n";
} catch (\OPcache\StaticCacheException $e) {
    echo "legacy: rejected\n";
}

/* Shared object identity is preserved on the fast path. */
class Node
{
    public ?Node $a = null;
    public ?Node $b = null;
    public int $n = 0;
}

$shared = new Node();
$shared->n = 9;
$root = new Node();
$root->a = $shared;
$root->b = $shared;
VC::getInstance('default')->store('k_shared', $root);
$g = VC::getInstance('default')->fetch('k_shared');
var_dump($g->a === $g->b);
echo "shared n={$g->a->n}\n";

/* Cycles are preserved on the fast path. */
$cycle = new Node();
$cycle->n = 5;
$cycle->a = $cycle;
VC::getInstance('default')->store('k_cycle', $cycle);
$gc = VC::getInstance('default')->fetch('k_cycle');
var_dump($gc->a === $gc);
echo "cycle n={$gc->n}\n";
?>
--EXPECT--
serialize calls: 1
v=42 tag=t
unserialize calls: 1
bool(true)
legacy: rejected
bool(true)
shared n=9
bool(true)
cycle n=5

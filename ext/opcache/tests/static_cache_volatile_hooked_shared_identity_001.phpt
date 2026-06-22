--TEST--
OPcache volatile cache: __serialize objects shared across a graph (or cyclic through their own state) keep one identity on the fast path, with __serialize called once
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

class K_Hooked
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

class K_Box
{
    public ?object $a = null;
    public ?object $b = null;
}

/* A __serialize object referenced twice stays on the fast path: __serialize runs
 * once on store, the two slots decode to one instance, and a mutation through one
 * is visible through the other. */
$h = new K_Hooked(7);
$box = new K_Box();
$box->a = $h;
$box->b = $h;

K_Hooked::$ser = 0;
VC::getInstance('default')->store('k_shared', $box);
echo "serialize calls: ", K_Hooked::$ser, "\n";

$g = VC::getInstance('default')->fetch('k_shared');
echo "identity a===b: ";
var_dump($g->a === $g->b);
$g->a->v = 99;
echo "mutation is shared: ";
var_dump($g->b->v === 99);

/* Each fetch is an independent clone, itself internally consistent. */
$g2 = VC::getInstance('default')->fetch('k_shared');
echo "fetches are independent: ";
var_dump($g->a !== $g2->a);
echo "second fetch identity: ";
var_dump($g2->a === $g2->b);

/* The same instance reached from both an array entry and an object property. */
$h2 = new K_Hooked(1);
$box2 = new K_Box();
$box2->a = $h2;
$box2->b = $h2;
VC::getInstance('default')->store('k_multi', ['x' => $h2, 'y' => $box2]);
$m = VC::getInstance('default')->fetch('k_multi');
echo "all one instance: ";
var_dump($m['x'] === $m['y']->a && $m['y']->a === $m['y']->b);

/* A cycle through a __serialize object's own state survives on the fast path. */
class K_Self
{
    public ?object $self = null;
    public int $v = 0;

    public function __serialize(): array
    {
        return ['v' => $this->v, 'self' => $this->self];
    }

    public function __unserialize(array $data): void
    {
        $this->v = $data['v'];
        $this->self = $data['self'];
    }
}

$s = new K_Self();
$s->v = 5;
$s->self = $s;
VC::getInstance('default')->store('k_selfcycle', $s);
$gs = VC::getInstance('default')->fetch('k_selfcycle');
echo "self-cycle preserved: ";
var_dump($gs->self === $gs);
echo "selfcycle v=", $gs->v, "\n";
?>
--EXPECT--
serialize calls: 1
identity a===b: bool(true)
mutation is shared: bool(true)
fetches are independent: bool(true)
second fetch identity: bool(true)
all one instance: bool(true)
self-cycle preserved: bool(true)
selfcycle v=5

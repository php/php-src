--TEST--
OPcache volatile cache: hard references (&) inside arrays use a transparent shared-graph node, preserving aliasing and shared identity, including references held by object properties
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

/* 1. A shared scalar reference inside an array stays on the fast path and keeps
 * its aliasing: writing through one alias is seen through the other, while each
 * fetch is an independent clone. */
$a = ['value' => 1, 'tag' => 't'];
$a['alias'] = &$a['value'];
var_dump(VC::getInstance('default')->store('k_scalar', $a) === true);

$g = VC::getInstance('default')->fetch('k_scalar');
$g['value'] = 99;
echo "scalar alias follows write: ";
var_dump($g['alias'] === 99);
echo "scalar other fetch is independent: ";
var_dump(VC::getInstance('default')->fetch('k_scalar')['value'] === 1);

/* 2. A single (unshared) reference still round-trips through the fast path. */
$x = 10;
$b = ['r' => &$x, 'tag' => 'u'];
var_dump(VC::getInstance('default')->store('k_single', $b) === true);
echo "single value: ";
var_dump(VC::getInstance('default')->fetch('k_single')['r'] === 10);

/* 3. A reference to a userland object, shared across two array slots, keeps both
 * the reference aliasing and the object identity. */
class K_Holder
{
    public int $n = 0;
}

$o = new K_Holder();
$o->n = 7;
$c = [];
$c['p'] = &$o;
$c['q'] = &$o;
var_dump(VC::getInstance('default')->store('k_objref', $c) === true);

$gc = VC::getInstance('default')->fetch('k_objref');
echo "objref identity: ";
var_dump($gc['p'] === $gc['q']);
$gc['p']->n = 123;
echo "objref shared mutation: ";
var_dump($gc['q']->n === 123);

/* 4. A reference held by an object property is preserved on the fast path: the
 * reference is installed into the property slot (with a type source for typed
 * properties), so aliasing survives. */
class K_Pair
{
    public $a;
    public $b;
}

$pair = new K_Pair();
$v = 1;
$pair->a = &$v;
$pair->b = &$v;
var_dump(VC::getInstance('default')->store('k_proptref', $pair) === true);
echo "proptref round-trips to object: ";
var_dump(VC::getInstance('default')->fetch('k_proptref') instanceof K_Pair);

$gp = VC::getInstance('default')->fetch('k_proptref');
$gp->a = 55;
echo "proptref alias preserved: ";
var_dump($gp->b === 55);
?>
--EXPECT--
bool(true)
scalar alias follows write: bool(true)
scalar other fetch is independent: bool(true)
bool(true)
single value: bool(true)
bool(true)
objref identity: bool(true)
objref shared mutation: bool(true)
bool(true)
proptref round-trips to object: bool(true)
proptref alias preserved: bool(true)

--TEST--
OPcache volatile cache: references held by dynamic (non-declared) object properties are preserved on the fast path
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

/* A scalar reference shared across two dynamic properties of a stdClass. */
$o = new stdClass();
$v = 1;
$o->a = &$v;
$o->b = &$v;
VC::getInstance('default')->store('dpr_scalar', $o);
$g = VC::getInstance('default')->fetch('dpr_scalar');
$g->a = 99;
echo "scalar alias: ";
var_dump($g->b === 99);
echo "independent fetch: ";
var_dump(VC::getInstance('default')->fetch('dpr_scalar')->a === 1);

/* A reference to a shared object across two dynamic properties. */
$shared = (object) ['n' => 0];
$w = new stdClass();
$w->x = &$shared;
$w->y = &$shared;
VC::getInstance('default')->store('dpr_obj', $w);
$gw = VC::getInstance('default')->fetch('dpr_obj');
echo "object identity: ";
var_dump($gw->x === $gw->y);
$gw->x->n = 7;
echo "object shared mutation: ";
var_dump($gw->y->n === 7);
?>
--EXPECT--
scalar alias: bool(true)
independent fetch: bool(true)
object identity: bool(true)
object shared mutation: bool(true)

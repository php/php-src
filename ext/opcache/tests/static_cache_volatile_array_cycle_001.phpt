--TEST--
OPcache volatile cache: cyclic arrays (always closed through a &) are kept on the fast path, with the cycle and value semantics matching native PHP
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

/* A self-referential array survives on the fast path: the cycle resolves to the
 * same array, and nested values come back intact. */
$c = ['n' => 5, 'list' => [1, 2, 3]];
$c['self'] = &$c;
VC::getInstance('default')->store('k_self', $c);
$g = VC::getInstance('default')->fetch('k_self');
echo "self cycle resolves: ";
var_dump($g['self'] === $g);
echo "value through cycle: ", $g['self']['n'], " ", $g['self']['list'][1], "\n";

/* Two arrays referencing each other. */
$p = ['name' => 'p'];
$q = ['name' => 'q'];
$p['other'] = &$q;
$q['other'] = &$p;
VC::getInstance('default')->store('k_mutual', $p);
$gm = VC::getInstance('default')->fetch('k_mutual');
echo "mutual cycle resolves: ";
var_dump($gm['other']['other'] === $gm);
echo "names: ", $gm['name'], " ", $gm['other']['name'], "\n";

/* A back-edge several levels deep. */
$root = ['a' => ['b' => ['c' => 7]]];
$root['a']['b']['back'] = &$root;
VC::getInstance('default')->store('k_deep', $root);
$gd = VC::getInstance('default')->fetch('k_deep');
echo "deep cycle resolves: ";
var_dump($gd['a']['b']['back'] === $gd);
echo "deep value: ", $gd['a']['b']['back']['a']['b']['c'], "\n";

/* Each fetch is an independent clone: mutating one leaves the other untouched. */
$g1 = VC::getInstance('default')->fetch('k_self');
$g2 = VC::getInstance('default')->fetch('k_self');
$g1['n'] = 99;
echo "fetches independent: ";
var_dump($g2['n'] === 5);

/* A fresh fetch still carries its own intact cycle. */
$g3 = VC::getInstance('default')->fetch('k_self');
echo "later fetch cycle intact: ";
var_dump($g3['self'] === $g3);
?>
--EXPECT--
self cycle resolves: bool(true)
value through cycle: 5 2
mutual cycle resolves: bool(true)
names: p q
deep cycle resolves: bool(true)
deep value: 7
fetches independent: bool(true)
later fetch cycle intact: bool(true)

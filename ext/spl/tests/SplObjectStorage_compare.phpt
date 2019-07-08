--TEST--
SPL: SplObjectStorage: `==` comparison
--FILE--
<?php
$o = new SplObjectStorage();
$p = new SplObjectStorage();
$q = new SplObjectStorage();

$obj = new StdClass;

$o[$obj] = 0;
$p[$obj] = 0;
$q[$obj] = null;

var_dump($o == $p);
var_dump($o == $q);
--EXPECT--
bool(true)
bool(true)
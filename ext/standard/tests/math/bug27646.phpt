--TEST--
Bug #27646 (Cannot serialize/unserialize non-finite numeric values)
--FILE--
<?php

$f=-INF;
var_dump($f);
var_dump(serialize($f));
var_dump(unserialize(serialize($f)));

$f=INF;
var_dump($f);
var_dump(serialize($f));
var_dump(unserialize(serialize($f)));

$f=NAN;
var_dump($f);
var_dump(serialize($f));
var_dump(unserialize(serialize($f)));

?>
--EXPECT--
float(-INF)
string(7) "d:-INF;"
float(-INF)
float(INF)
string(6) "d:INF;"
float(INF)
float(NAN)
string(6) "d:NAN;"
float(NAN)

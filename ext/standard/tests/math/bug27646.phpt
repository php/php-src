--TEST--
Bug #27646 (Cannot serialize/unserialize non-finite numeric values)
--FILE--
<?php
set_time_limit(5);

$f=12.3;
var_dump($f);
var_dump(serialize($f));
var_dump(unserialize(serialize($f)));

$f=-12.3;
var_dump($f);
var_dump(serialize($f));
var_dump(unserialize(serialize($f)));

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
--EXPECTF--
float(1%f)
string(%d) "d:1%s;"
float(1%f)
float(-1%f)
string(%d) "d:-1%s;"
float(-1%f)
float(-INF)
string(7) "d:-INF;"
float(-INF)
float(INF)
string(6) "d:INF;"
float(INF)
float(NAN)
string(6) "d:NAN;"
float(NAN)

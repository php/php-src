--TEST--
Bug #71314 (var_export(INF) prints INF.0)
--FILE--
<?php

var_export(INF);
echo PHP_EOL;
var_export(-INF);
echo PHP_EOL;
var_export(NAN);
echo PHP_EOL;
--EXPECT--
INF
-INF
NAN

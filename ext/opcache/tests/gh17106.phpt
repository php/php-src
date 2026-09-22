--TEST--
GH-17106: ZEND_MATCH_ERROR misoptimization
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

const X = 2;

var_dump(7 ?? match (X) {});
var_dump(null ?? match (X) { 2 => 2 });
var_dump(match (X) { 2 => 2 });

?>
--EXPECT--
int(7)
int(2)
int(2)

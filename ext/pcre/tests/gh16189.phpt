--TEST--
GH-16189 (preg_match/preg_match_all underflow on start_offset argument)
--FILE--
<?php

var_dump(preg_match( '/<(\w+)[\s\w\-]+ id="S44_i89ew">/', '<br><div id="S44_i89ew">', $matches, 0, PHP_INT_MIN));
var_dump(preg_match_all( '/<(\w+)[\s\w\-]+ id="S44_i89ew">/', '<br><div id="S44_i89ew">', $matches, 0, PHP_INT_MIN));
?>
--EXPECT--
int(1)
int(1)

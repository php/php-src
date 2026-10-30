--TEST--
preg_match_all() resource cleanup when \K in lookahead causes negative-length match
--FILE--
<?php
$result = preg_match_all('/(?=ab\K)a/', 'ab', $matches);
var_dump($result);
?>
--EXPECTF--
Warning: preg_match_all(): Get subpatterns list failed in %s on line %d
bool(false)

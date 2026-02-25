--TEST--
preg_match() resource cleanup when \K in lookahead causes negative-length match
--FILE--
<?php
$result = preg_match('/(?=ab\K)a/', 'ab', $matches);
var_dump($result);
?>
--EXPECTF--
Warning: preg_match(): Get subpatterns list failed in %s on line %d
bool(false)

--TEST--
Bug #33605 (substr_compare crashes)
--FILE--
<?php
$res = substr_compare("aa", "a", -99999999, -1, 0);
var_dump($res);

?>
--EXPECTF--
Warning: substr_compare(): The length must be greater than or equal to zero in %s on line %d
bool(false)

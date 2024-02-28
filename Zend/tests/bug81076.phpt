--TEST--
Bug #81076 Invalid implicit binds cause incorrect static var count in closure debug info
--FILE--
<?php
var_dump(fn() => [$why, $do, $we, $count]);
?>
--EXPECTF--
object(Closure)#%d (1) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
}

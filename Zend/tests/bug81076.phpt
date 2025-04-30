--TEST--
Bug #81076 Invalid implicit binds cause incorrect static var count in closure debug info
--FILE--
<?php
var_dump(fn() => [$why, $do, $we, $count]);
?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}

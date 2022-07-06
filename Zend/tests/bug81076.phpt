--TEST--
Bug #81076 Invalid implicit binds cause incorrect static var count in closure debug info
--FILE--
<?php
var_dump(fn() => [$why, $do, $we, $count]);
?>
--EXPECT--
object(Closure)#1 (0) {
}

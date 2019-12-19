--TEST--
Bug #78999 (Cycle leak when using function result as temporary)
--FILE--
<?php
function get() {
	$t = new stdClass;
	$t->prop = $t;
	return $t;
}
var_dump(get());
var_dump(gc_collect_cycles());
--EXPECT--
object(stdClass)#1 (1) {
  ["prop"]=>
  *RECURSION*
}
int(1)

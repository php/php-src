--TEST--
Bug #78999 (Cycle leak when using function result as temporary) — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
function get() {
    $t = new stdClass;
    $t->prop = $t;
    return $t;
}
var_dump(get());
var_dump(gc_collect_cycles());
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["prop"]=>
  *RECURSION*
}
int(1)

--TEST--
Object properties HT may need to be removed from nested data — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

#[AllowDynamicProperties]
class Test {
    public function __destruct() {
        $GLOBALS['x'] = $this;
    }
}

$t = new Test;
$t->x = new stdClass;
$t->x->t = $t;
$a = (array) $t->x;
unset($t, $a);
gc_collect_cycles();
var_dump($x);

?>
--EXPECTF--
object(Test)#%d (1) {
  ["x"]=>
  object(stdClass)#%d (1) {
    ["t"]=>
    *RECURSION*
  }
}

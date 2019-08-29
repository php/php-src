--TEST--
Object properties HT may need to be removed from nested data
--FILE--
<?php

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
--EXPECT--
object(Test)#1 (1) {
  ["x"]=>
  object(stdClass)#2 (1) {
    ["t"]=>
    *RECURSION*
  }
}

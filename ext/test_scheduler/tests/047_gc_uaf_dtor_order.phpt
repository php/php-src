--TEST--
Bug #72530: Use After Free in GC with Certain Destructors — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class ryat {
    var $ryat;
    var $chtg;

    function __destruct() {
        $this->chtg = $this->ryat;
        $this->ryat = 1;
    }
}

$o = new ryat;
$o->ryat = $o;
$x =& $o->chtg;

unset($o);
gc_collect_cycles();
var_dump($x);

?>
--EXPECTF--
object(ryat)#%d (2) {
  ["ryat"]=>
  int(1)
  ["chtg"]=>
  *RECURSION*
}

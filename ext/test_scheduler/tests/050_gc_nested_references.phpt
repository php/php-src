--TEST--
GC 041: Handling of references in nested data of objects with destructor — under test_scheduler
--INI--
test_scheduler.enable=1
zend.enable_gc = 1
--EXTENSIONS--
test_scheduler
--FILE--
<?php
class ryat {
        var $ryat;
        var $chtg;
        var $nested;
        function __destruct() {
                $GLOBALS['x'] = $this;
        }
}
$o = new ryat;
$o->nested = [];
$o->nested[] =& $o->nested;
$o->ryat = $o;
$x =& $o->chtg;
unset($o);
var_dump(gc_collect_cycles());
var_dump($x);
?>
--EXPECTF--
int(0)
object(ryat)#%d (3) {
  ["ryat"]=>
  *RECURSION*
  ["chtg"]=>
  *RECURSION*
  ["nested"]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}

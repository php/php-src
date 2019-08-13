--TEST--
GC 041: Handling of references in nested data of objects with destructor
--INI--
zend.enable_gc = 1
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
--EXPECT--
int(0)
object(ryat)#1 (3) {
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

--TEST--
References part of nested data of object with destructor
--FILE--
<?php

class ryat {
    var $ryat;
    var $nested;

    function __destruct() {
        $GLOBALS['x'] = $this;
    }
}

$o = new ryat;
$o->nested = [];
$o->nested[] =& $o->nested;
$o->ryat = $o;

unset($o);
gc_collect_cycles();
var_dump($x);
unset($x);
gc_collect_cycles();

?>
--EXPECT--
object(ryat)#1 (2) {
  ["ryat"]=>
  *RECURSION*
  ["nested"]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}

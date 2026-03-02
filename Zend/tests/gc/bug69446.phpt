--TEST--
Bug #69446 (GC leak relating to removal of nested data after dtors run)
--INI--
zend.enable_gc = 1
--FILE--
<?php
$bar = NULL;
#[AllowDynamicProperties]
class bad {
    public function __destruct() {
        global $bar;
        $bar = $this;
        $bar->y = new stdClass;
    }
}

$foo = new stdClass;
$foo->foo = $foo;
$foo->bad = new bad;
$foo->bad->x = new stdClass;

unset($foo);
gc_collect_cycles();
var_dump($bar);
?>
--EXPECTF--
object(bad)#%d (2) {
  ["x"]=>
  object(stdClass)#%d (0) {
  }
  ["y"]=>
  object(stdClass)#%d (0) {
  }
}

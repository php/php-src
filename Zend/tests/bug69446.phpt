--TEST--
Bug #69446 (GC leak relating to removal of nested data after dtors run)
--INI--
zend.enable_gc = 1
--FILE--
<?php
$bar = NULL;
class bad {
	public function __destruct() {
		global $bar;
		$bar = $this;
		$bar->y = new StdClass;
	}
}

$foo = new StdClass;
$foo->foo = $foo;
$foo->bad = new bad;
$foo->bad->x = new StdClass;

unset($foo);
gc_collect_cycles();
var_dump($bar);
--EXPECT--
object(bad)#2 (2) {
  ["x"]=>
  object(StdClass)#3 (0) {
  }
  ["y"]=>
  object(StdClass)#4 (0) {
  }
}

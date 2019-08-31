--TEST--
GC 016: nested GC calls
--INI--
zend.enable_gc=1
--FILE--
<?php
class Foo {
	public $a;
	function __destruct() {
		echo "-> ";
		$a = array();
		$a[] =& $a;
		unset($a);
		var_dump(gc_collect_cycles());
	}
}
$a = new Foo();
$a->a = $a;
unset($a);
var_dump(gc_collect_cycles());
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
-> int(0)
int(0)
int(2)
ok

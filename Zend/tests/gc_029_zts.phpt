--TEST--
GC 029: GC and destructors
--SKIPIF--
<?php if (!PHP_ZTS) { print "skip only for zts build"; }
--INI--
zend.enable_gc=1
--FILE--
<?php
class Foo {
	public $bar;
	public $x = array(1,2,3);
	function __destruct() {
		if ($this->bar !== null) {
			$this->x = null;
			unset($this->bar);
		}
	}
}
class Bar {
	public $foo;
        function __destruct() {
                if ($this->foo !== null) {
                        unset($this->foo);
                }
        }

}
$foo = new Foo();
$bar = new Bar();
$foo->bar = $bar;
$bar->foo = $foo;
unset($foo);
unset($bar);
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(6)

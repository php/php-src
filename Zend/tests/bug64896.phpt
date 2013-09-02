--TEST--
Bug #64896 (Segfault with gc_collect_cycles using unserialize on certain objects)
--XFAIL--
We can not fix this bug without a significant (performace slow down) change to gc
--FILE--
<?php
$bar = NULL;
class bad
{
	private $_private = array();

	public function __construct()
	{
		$this->_private[] = 'php';
	}

	public function __destruct()
	{
		global $bar;
		$bar = $this;
	}
}

$foo = new stdclass;
$foo->foo = $foo;
$foo->bad = new bad;

gc_disable();

unserialize(serialize($foo));
gc_collect_cycles();
var_dump($bar); 
/*  will output:
object(bad)#4 (1) {
  ["_private":"bad":private]=>
  &UNKNOWN:0
}
*/
?>
--EXPECTF--
bject(bad)#%d (1) {
  ["_private":"bad":private]=>
  array(1) {
    [0]=>
    string(3) "php"
  }
}

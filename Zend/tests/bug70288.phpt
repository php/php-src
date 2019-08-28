--TEST--
Bug #70288 (Apache crash related to ZEND_SEND_REF)
--FILE--
<?php
class A {
	public function __get($name) {
		return new StdClass();
	}
}

function test(&$obj) {
	var_dump($obj);
}
$a = new A;
test($a->dummy);
?>
--EXPECTF--
object(StdClass)#%d (0) {
}

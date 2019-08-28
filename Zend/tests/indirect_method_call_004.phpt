--TEST--
Indirect method call and cloning
--FILE--
<?php


class bar {
	public $z;

	public function __construct() {
		$this->z = new StdClass;
	}
	public function getZ() {
		return $this->z;
	}
}

var_dump(clone (new bar)->z);
var_dump(clone (new bar)->getZ());

?>
--EXPECTF--
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}

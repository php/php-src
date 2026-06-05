--TEST--
Indirect method call and cloning
--FILE--
<?php


class bar {
    public $z;

    public function __construct() {
        $this->z = new stdclass;
    }
    public function getZ() {
        return $this->z;
    }
}

var_dump(clone (new bar)->z);
var_dump(clone (new bar)->getZ());

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "stdclass" not found in %s:%d
Stack trace:
#0 %s(%d): bar->__construct()
#1 {main}
  thrown in %s on line %d

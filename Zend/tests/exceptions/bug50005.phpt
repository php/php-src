--TEST--
Bug #50005 (Throwing through Reflection modified Exception object makes segmentation fault)
--FILE--
<?php

class a extends Exception {
    public function __construct() {
        $this->file = "";
    }
}

throw new a;

?>
--EXPECTF--
Fatal error: Uncaught a in :%d
Stack trace:
#0 {main}
  thrown in Unknown on line %d

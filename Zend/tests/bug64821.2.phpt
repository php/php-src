--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 2)
--FILE--
<?php

class a extends exception {
    public function __construct() {
        $this->line = array();
    }
}

throw new a;

?>
--EXPECTF--
Fatal error: Uncaught a in %s:0
Stack trace:
#0 {main}
  thrown in %s on line %d

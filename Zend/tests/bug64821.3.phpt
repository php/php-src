--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 3)
--FILE--
<?php

class a extends exception {
    public function __construct() {
        $this->line = 0;
        $this->file = "";
    }
}

throw new a;

?>
--EXPECTF--
Fatal error: Uncaught a in :0
Stack trace:
#0 {main}
  thrown in Unknown on line %d

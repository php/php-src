--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 1)
--FILE--
<?php

#[AllowDynamicProperties]
class a extends exception {
    public function __construct() {
        $this->message = NULL;
        $this->string  = NULL;
        $this->code    = array();
        $this->line = 0;
    }
}

throw new a;

?>
--EXPECTF--
Fatal error: Uncaught a in %s:0
Stack trace:
#0 {main}
  thrown in %s on line %d

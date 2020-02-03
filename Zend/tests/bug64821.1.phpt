--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 1)
--FILE--
<?php

class a extends exception
{
    public function __construct()
    {
        $this->message = null;
        $this->string  = null;
        $this->code    = array();
        $this->line = "hello";
    }
}

throw new a();

?>
--EXPECTF--
Fatal error: Uncaught a in %s:0
Stack trace:
#0 {main}
  thrown in %s on line %d

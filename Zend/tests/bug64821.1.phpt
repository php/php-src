--TEST--
Bug #64821 Custom Exceptions crash when internal properties overridden (variation 1)
--FILE--
<?php

class a extends exception {
	public function __construct() {
		$this->message = NULL;
		$this->string  = NULL;
		$this->code    = array();
		$this->line = "hello";
	}
}

throw new a;

?>
--EXPECTF--
Fatal error: Uncaught exception 'a' in %s:0
Stack trace:
#0 {main}
  thrown in %s on line %d

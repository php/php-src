--TEST--
#[\NonpublicConstructor]: affects error message (protected constructor, global scope)
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor("use ::getInstance() instead")]
    protected function __construct() {}
}

new Demo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to protected Demo::__construct() from global scope, use ::getInstance() instead, in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %s

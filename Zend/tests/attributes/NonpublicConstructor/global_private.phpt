--TEST--
#[\NonpublicConstructor]: affects error message (private constructor, global scope)
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor("use ::getInstance() instead")]
    private function __construct() {}
}

new Demo();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private Demo::__construct() from global scope, use ::getInstance() instead, in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %s

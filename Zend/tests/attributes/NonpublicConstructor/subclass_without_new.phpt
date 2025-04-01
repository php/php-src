--TEST--
#[\NonpublicConstructor]: affects error message (private constructor, protected scope, called via `parent::__construct()`)
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor("use ::getInstance() instead")]
    private function __construct() {}
}

class Subclass extends Demo {
    public function __construct() {
        parent::__construct();
    }
}

new Subclass();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call private Demo::__construct(), use ::getInstance() instead, in %s:%d
Stack trace:
#0 %s(%d): Subclass->__construct()
#1 {main}
  thrown in %s on line %s

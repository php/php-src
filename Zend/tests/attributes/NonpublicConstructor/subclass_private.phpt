--TEST--
#[\NonpublicConstructor]: affects error message (private constructor, protected scope)
--FILE--
<?php

class Demo {
    #[\NonpublicConstructor("use ::getInstance() instead")]
    private function __construct() {}
}

class Subclass extends Demo {
    public static function create() {
        return new Demo();
    }
}

Subclass::create();

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private Demo::__construct() from scope Subclass, use ::getInstance() instead, in %s:%d
Stack trace:
#0 %s(%d): Subclass::create()
#1 {main}
  thrown in %s on line %s

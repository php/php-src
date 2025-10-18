--TEST--
Pass short ctor parameters
--FILE--
<?php

try {
    assert(false && function () {

	abstract class Family($string);

	class Child1($string) extends Family () {}
	class Child2($string) extends Family (555) {}

    });
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
assert(false && function () {
    abstract class Family {
        public function __construct($string) {
        }

    }

    class Child1 extends Family {
        public function __construct($string) {
            parent::__construct();
        }

    }

    class Child2 extends Family {
        public function __construct($string) {
            parent::__construct(555);
        }

    }

})
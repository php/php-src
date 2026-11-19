--TEST--
Pass short ctor parameters
--FILE--
<?php

try {
    assert(false && function () {

	abstract class Family(protected $protected);

	class Child1(public $string) extends Family ($string) {}


    });
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
assert(false && function () {
    abstract class Family {
        public function __construct(protected $protected) {
        }

    }

    class Child1 extends Family {
        public function __construct(public $string) {
            parent::__construct($string);
        }

    }

})
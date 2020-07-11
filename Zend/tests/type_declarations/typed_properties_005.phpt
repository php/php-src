--TEST--
Test typed properties error condition (type mismatch object)
--FILE--
<?php
class Dummy {}

new class(new Dummy) {
    public stdClass $std;

    public function __construct(Dummy $dummy) {
        $this->std = $dummy;
    }
};
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign Dummy to property class@anonymous::$std of type stdClass in %s:%d
Stack trace:
#0 %s(%d): class@anonymous->__construct(Object(Dummy))
#1 {main}
  thrown in %s on line %d

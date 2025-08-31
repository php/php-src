--TEST--
Testing visibility of methods
--FILE--
<?php

class d {
    private function test2() {
        print "Bar\n";
    }
}

abstract class a extends d {
    public function test() {
        $this->test2();
    }
}

abstract class b extends a {
}

class c extends b {
    public function __construct() {
        $this->test();
    }
}

new c;

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private method d::test2() from scope a in %s:%d
Stack trace:
#0 %s(%d): a->test()
#1 %s(%d): c->__construct()
#2 {main}
  thrown in %s on line %d

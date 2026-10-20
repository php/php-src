--TEST--
GH-16198: Usage of super in cloned trait method
--FILE--
<?php

trait T {
    public function __destruct() {
        parent::__destruct();
    }
}

class P {
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

class C extends P {
    use T;
}

$c = new C();
unset($c);

?>
--EXPECT--
string(13) "P::__destruct"

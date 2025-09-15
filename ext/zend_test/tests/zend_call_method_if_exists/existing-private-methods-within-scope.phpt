--TEST--
zend_call_method_if_exists() with existing public method
--EXTENSIONS--
zend_test
--FILE--
<?php

class C {
    private function priv() {
        var_dump(__METHOD__);
    }
    public function test() {
        // this should call $c->priv()
        zend_call_method_if_exists($this, 'priv');
    }
}

$c = new C();
$c->test();

?>
--EXPECT--
string(7) "C::priv"

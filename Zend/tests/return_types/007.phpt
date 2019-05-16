--TEST--
Return value is subclass of return type
--FILE--
<?php
class foo {}

class qux extends foo {
    public function foo() : foo {
        return $this;
    }
}

$qux = new qux();
var_dump($qux->foo());
--EXPECTF--
object(qux)#%d (%d) {
}

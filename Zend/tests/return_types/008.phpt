--TEST--
Return type covariance in interface implementation

--FILE--
<?php
interface foo {
    public function bar() : foo;
}


class qux implements foo {
    public function bar() : qux {
        return $this;
    }
}

$qux = new qux();
var_dump($qux->bar());

--EXPECTF--
Fatal error: Declaration of qux::bar() must be compatible with foo::bar(): foo in %s008.php on line 7

--TEST--
Return type covariance error

--FILE--
<?php
interface foo {
    public function bar() : foo;
}

interface biz {}

class qux implements foo {
    public function bar() : biz {
        return $this;
    }
}

--EXPECTF--
Fatal error: Declaration of qux::bar() must be compatible with foo::bar(): foo in %s on line %d

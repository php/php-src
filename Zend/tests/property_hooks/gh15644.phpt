--TEST--
GH-15644: Asymmetric visibility doesn't work for set hook
--FILE--
<?php

class C {
    public private(set) string $prop {
        set => $value;
    }
}

$c = new C();
$c->prop = 'hello world';

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify private(set) property C::$prop from global scope in %s:%d
%a

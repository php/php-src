--TEST--
Cannot use void as a constraint
--FILE--
<?php

interface I<T : void> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot use static, void, or never to constrain generic parameter T in %s on line %d

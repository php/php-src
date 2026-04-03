--TEST--
Cannot use never as a constraint
--FILE--
<?php

interface I<T : never> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot use static, void, or never to constrain generic parameter T in %s on line %d

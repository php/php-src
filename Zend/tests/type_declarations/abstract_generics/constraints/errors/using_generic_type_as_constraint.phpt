--TEST--
Cannot use generic type as a constraint
--FILE--
<?php

interface I<T1, T2 : T1> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot use generic parameter T1 to constrain generic parameter T2 in %s on line %d

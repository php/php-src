--TEST--
OSS-Fuzz #429429090: FETCH_OBJ_UNSET IS_UNDEF result
--FILE--
<?php

class C {
    public D $x;
    static D $y;
}

$c = new C();
isset($c->x[0]->prop);
unset($c->x[0]->prop);
isset(C::$y[0]->prop);
unset(C::$y[0]->prop);

?>
===DONE===
--EXPECT--
===DONE===
